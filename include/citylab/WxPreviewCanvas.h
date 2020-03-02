#pragma once

#include <ee3/WxStageCanvas.h>

namespace citylab
{

class WxEditorPanel;

class WxPreviewCanvas : public ee3::WxStageCanvas
{
public:
    WxPreviewCanvas(ee0::WxStagePage* stage, ECS_WORLD_PARAM
        const ee0::RenderContext& rc);

protected:
    virtual void DrawBackground3D() const override;
    virtual void DrawForeground3D() const override;
    virtual void DrawForeground2D() const override;

}; // WxPreviewCanvas

}