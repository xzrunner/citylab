#include "citylab/PreviewPage.h"
#include "citylab/WxPreviewCanvas.h"

#include <ee0/WxStagePage.h>
#include <ee3/WxStageCanvas.h>
#include <ee3/WorldTravelOP.h>

namespace citylab
{

PreviewPage::PreviewPage(const ur::Device& dev, ee0::WxStagePage& stage_page,
                         const ee0::RenderContext& rc)
    : m_stage_page(stage_page)
{
    Init(dev, rc);
}

PreviewPage::~PreviewPage()
{
}

void PreviewPage::OnNotify(uint32_t msg, const ee0::VariantSet& variants)
{
}

void PreviewPage::Init(const ur::Device& dev, const ee0::RenderContext& rc)
{
    auto canvas = std::make_shared<WxPreviewCanvas>(dev, &m_stage_page, ECS_WORLD_VAR rc);
    m_stage_page.GetImpl().SetCanvas(canvas);

    auto cam = canvas->GetCamera();
    auto& vp = std::static_pointer_cast<ee3::WxStageCanvas>(canvas)->GetViewport();
    auto sub_mgr = m_stage_page.GetSubjectMgr();
    auto op = std::make_shared<ee3::WorldTravelOP>(cam, vp, sub_mgr);
    m_stage_page.GetImpl().SetEditOP(op);

    canvas->InitEditOP(op);
}

}