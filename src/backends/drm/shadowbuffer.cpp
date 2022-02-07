/*
    KWin - the KDE window manager
    This file is part of the KDE project.

    SPDX-FileCopyrightText: 2021 Xaver Hugl <xaver.hugl@gmail.com>

    SPDX-License-Identifier: GPL-2.0-or-later
*/
#include "shadowbuffer.h"

#include "logging.h"
#include "drm_output.h"

namespace KWin
{

static const float vertices[] = {
   -1.0f,  1.0f,
   -1.0f, -1.0f,
    1.0f, -1.0f,

   -1.0f,  1.0f,
    1.0f, -1.0f,
    1.0f,  1.0f,
};

static const float texCoords[] = {
    0.0f,  1.0f,
    0.0f,  0.0f,
    1.0f,  0.0f,

    0.0f,  1.0f,
    1.0f,  0.0f,
    1.0f,  1.0f
};

ShadowBuffer::ShadowBuffer(const QSize &size, const GbmFormat &format)
    : m_size(size)
{
    m_texture.reset(new GLTexture(internalFormat(format), size));
    m_texture->setFilter(GL_NEAREST);

    m_renderTarget.reset(new GLRenderTarget(*m_texture));
    if (!m_renderTarget->valid()) {
        qCCritical(KWIN_DRM) << "Error: framebuffer not complete!";
        return;
    }

    m_vbo.reset(new GLVertexBuffer(KWin::GLVertexBuffer::Static));
    m_vbo->setData(6, 2, vertices, texCoords);
}

ShadowBuffer::~ShadowBuffer()
{
}

void ShadowBuffer::render(DrmAbstractOutput *output)
{
    QMatrix4x4 mvpMatrix;
    switch (output->transform()) {
    case DrmOutput::Transform::Normal:
    case DrmOutput::Transform::Flipped:
        break;
    case DrmOutput::Transform::Rotated90:
    case DrmOutput::Transform::Flipped90:
        mvpMatrix.rotate(90, 0, 0, 1);
        break;
    case DrmOutput::Transform::Rotated180:
    case DrmOutput::Transform::Flipped180:
        mvpMatrix.rotate(180, 0, 0, 1);
        break;
    case DrmOutput::Transform::Rotated270:
    case DrmOutput::Transform::Flipped270:
        mvpMatrix.rotate(270, 0, 0, 1);
        break;
    }
    switch (output->transform()) {
    case DrmOutput::Transform::Flipped:
    case DrmOutput::Transform::Flipped90:
    case DrmOutput::Transform::Flipped180:
    case DrmOutput::Transform::Flipped270:
        mvpMatrix.scale(-1, 1);
        break;
    default:
        break;
    }

    auto shader = ShaderManager::instance()->pushShader(ShaderTrait::MapTexture);
    shader->setUniform(GLShader::ModelViewProjectionMatrix, mvpMatrix);

    m_texture->bind();
    m_vbo->render(GL_TRIANGLES);
    ShaderManager::instance()->popShader();
}

GLRenderTarget *ShadowBuffer::renderTarget() const
{
    return m_renderTarget.data();
}

QSharedPointer<GLTexture> ShadowBuffer::texture() const
{
    return m_texture;
}

bool ShadowBuffer::isComplete() const
{
    return m_renderTarget->valid() && m_vbo;
}

GLint ShadowBuffer::internalFormat(const GbmFormat &format)
{
    if (format.redSize <= 8 && format.greenSize <= 8 && format.blueSize <= 8) {
        return GL_RGBA8;
    } else if (format.redSize <= 10 && format.greenSize <= 10 && format.blueSize <= 10) {
        return GL_RGB10_A2;
    } else if (format.redSize <= 12 && format.greenSize <= 12 && format.blueSize <= 12) {
        return GL_RGBA12;
    } else {
        return GL_RGBA16;
    }
}

}
