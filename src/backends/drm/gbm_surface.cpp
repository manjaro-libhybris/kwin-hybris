/*
    KWin - the KDE window manager
    This file is part of the KDE project.

    SPDX-FileCopyrightText: 2017 Martin Flöser <mgraesslin@kde.org>

    SPDX-License-Identifier: GPL-2.0-or-later
*/
#include "gbm_surface.h"

#include <errno.h>
#include <gbm.h>

#include "drm_backend.h"
#include "drm_gpu.h"
#include "egl_gbm_backend.h"
#include "kwineffects.h"
#include "kwineglutils_p.h"
#include "kwinglplatform.h"
#include "logging.h"

namespace KWin
{

GbmSurface::GbmSurface(DrmGpu *gpu, const QSize &size, uint32_t format, uint32_t flags, EGLConfig config)
    : m_surface(gbm_surface_create(gpu->gbmDevice(), size.width(), size.height(), format, flags))
    , m_eglBackend(static_cast<EglGbmBackend *>(gpu->platform()->renderBackend()))
    , m_size(size)
    , m_format(format)
    , m_fbo(new GLFramebuffer(0, size))
{
    if (!m_surface) {
        qCCritical(KWIN_DRM) << "Could not create gbm surface!" << strerror(errno);
        return;
    }
    m_eglSurface = eglCreatePlatformWindowSurfaceEXT(m_eglBackend->eglDisplay(), config, m_surface, nullptr);
    if (m_eglSurface == EGL_NO_SURFACE) {
        qCCritical(KWIN_DRM) << "Creating EGL surface failed!" << getEglErrorString();
    }
}

GbmSurface::GbmSurface(DrmGpu *gpu, const QSize &size, uint32_t format, QVector<uint64_t> modifiers, EGLConfig config)
    : m_surface(gbm_surface_create_with_modifiers(gpu->gbmDevice(), size.width(), size.height(), format, modifiers.isEmpty() ? nullptr : modifiers.constData(), modifiers.count()))
    , m_eglBackend(static_cast<EglGbmBackend *>(gpu->platform()->renderBackend()))
    , m_size(size)
    , m_format(format)
    , m_modifiers(modifiers)
    , m_fbo(new GLFramebuffer(0, size))
{
    if (!m_surface) {
        qCCritical(KWIN_DRM) << "Could not create gbm surface!" << strerror(errno);
        return;
    }
    m_eglSurface = eglCreatePlatformWindowSurfaceEXT(m_eglBackend->eglDisplay(), config, m_surface, nullptr);
    if (m_eglSurface == EGL_NO_SURFACE) {
        qCCritical(KWIN_DRM) << "Creating EGL surface failed!" << getEglErrorString();
    }
}

GbmSurface::~GbmSurface()
{
    auto buffers = m_lockedBuffers;
    for (auto buffer : buffers) {
        buffer->releaseBuffer();
    }
    if (m_eglSurface != EGL_NO_SURFACE) {
        eglDestroySurface(m_eglBackend->eglDisplay(), m_eglSurface);
    }
    if (m_surface) {
        gbm_surface_destroy(m_surface);
    }
}

bool GbmSurface::makeContextCurrent() const
{
    if (eglMakeCurrent(m_eglBackend->eglDisplay(), m_eglSurface, m_eglSurface, m_eglBackend->context()) == EGL_FALSE) {
        qCCritical(KWIN_DRM) << "eglMakeCurrent failed:" << getEglErrorString();
        return false;
    }
    if (!GLPlatform::instance()->isGLES()) {
        glDrawBuffer(GL_BACK);
    }
    return true;
}

QSharedPointer<DrmGbmBuffer> GbmSurface::swapBuffersForDrm(const QRegion &dirty)
{
    auto error = eglSwapBuffers(m_eglBackend->eglDisplay(), m_eglSurface);
    if (error != EGL_TRUE) {
        qCCritical(KWIN_DRM) << "an error occurred while swapping buffers" << getEglErrorString();
        return nullptr;
    }
    auto bo = gbm_surface_lock_front_buffer(m_surface);
    if (!bo) {
        return nullptr;
    }
    auto buffer = QSharedPointer<DrmGbmBuffer>::create(m_eglBackend->gpu(), this, bo);
    m_currentBuffer = buffer;
    m_lockedBuffers << m_currentBuffer.get();
    if (!buffer->bufferId()) {
        return nullptr;
    }
    m_currentDrmBuffer = buffer;
    if (m_eglBackend->supportsBufferAge()) {
        eglQuerySurface(m_eglBackend->eglDisplay(), m_eglSurface, EGL_BUFFER_AGE_EXT, &m_bufferAge);
        m_damageJournal.add(dirty);
    }
    return buffer;
}

QSharedPointer<GbmBuffer> GbmSurface::swapBuffers(const QRegion &dirty)
{
    auto error = eglSwapBuffers(m_eglBackend->eglDisplay(), m_eglSurface);
    if (error != EGL_TRUE) {
        qCCritical(KWIN_DRM) << "an error occurred while swapping buffers" << getEglErrorString();
        return nullptr;
    }
    auto bo = gbm_surface_lock_front_buffer(m_surface);
    if (!bo) {
        return nullptr;
    }
    m_currentBuffer = QSharedPointer<GbmBuffer>::create(this, bo);
    m_lockedBuffers << m_currentBuffer.get();
    if (m_eglBackend->supportsBufferAge()) {
        eglQuerySurface(m_eglBackend->eglDisplay(), m_eglSurface, EGL_BUFFER_AGE_EXT, &m_bufferAge);
        m_damageJournal.add(dirty);
    }
    return m_currentBuffer;
}

void GbmSurface::releaseBuffer(GbmBuffer *buffer)
{
    gbm_surface_release_buffer(m_surface, buffer->getBo());
    m_lockedBuffers.removeOne(buffer);
}

QSharedPointer<GbmBuffer> GbmSurface::currentBuffer() const
{
    return m_currentBuffer;
}

QSharedPointer<DrmGbmBuffer> GbmSurface::currentDrmBuffer() const
{
    return m_currentDrmBuffer;
}

GLFramebuffer *GbmSurface::fbo() const
{
    return m_fbo.data();
}

EGLSurface GbmSurface::eglSurface() const
{
    return m_eglSurface;
}

QSize GbmSurface::size() const
{
    return m_size;
}

bool GbmSurface::isValid() const
{
    return m_surface != nullptr && m_eglSurface != EGL_NO_SURFACE;
}

uint32_t GbmSurface::format() const
{
    return m_format;
}

QVector<uint64_t> GbmSurface::modifiers() const
{
    return m_modifiers;
}

int GbmSurface::bufferAge() const
{
    return m_bufferAge;
}

QRegion GbmSurface::repaintRegion() const
{
    if (m_eglBackend->supportsBufferAge()) {
        return m_damageJournal.accumulate(m_bufferAge, infiniteRegion());
    } else {
        return infiniteRegion();
    }
}

}
