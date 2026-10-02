#include "Viewport.h"

void Viewport::VlInitViewport(const VlViewport& viewport) noexcept
{
    m_viewport.TopLeftX = 0.0f;
    m_viewport.TopLeftY = 0.0f;
    m_viewport.Width = static_cast<float>(viewport.m_currentWidth);
    m_viewport.Height = static_cast<float>(viewport.m_currentHeight);
    m_viewport.MinDepth = 0.0f;
    m_viewport.MaxDepth = 1.0f;
}

void Viewport::VlUpdateViewport(const VlViewport& viewport) noexcept
{
    m_viewport.Width = static_cast<float>(viewport.m_NewWidth);
    m_viewport.Height = static_cast<float>(viewport.m_NewHeight);
}

void Viewport::Bind(ID3D11DeviceContext* context) const noexcept
{
    if (context != nullptr) {
        context->RSSetViewports(1, &m_viewport);
    }
}

float Viewport::AspectRatio() const noexcept
{
    if (m_viewport.Height <= 0.0f) {
        return 1.0f;
    }

    return m_viewport.Width / m_viewport.Height;
}
