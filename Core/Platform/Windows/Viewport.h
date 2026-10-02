#pragma once

#include <d3d11.h>

struct VlViewport {
    unsigned int m_currentWidth = 0;
    unsigned int m_currentHeight = 0;
    unsigned int m_NewWidth = 0;
    unsigned int m_NewHeight = 0;
};

class Viewport {
public:
    void VlInitViewport(const VlViewport& viewport) noexcept;
    void VlUpdateViewport(const VlViewport& viewport) noexcept;
    void Bind(ID3D11DeviceContext* context) const noexcept;

    [[nodiscard]] float AspectRatio() const noexcept;

private:
    D3D11_VIEWPORT m_viewport{};
};
