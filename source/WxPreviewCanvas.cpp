#include "citylab/WxPreviewCanvas.h"
#include "citylab/PreviewPage.h"
#include "citylab/Node.h"
#include "citylab/ShortestPathOP.h"
#include "citylab/RegistNodes.h"

#include <ee0/WxStagePage.h>
#include <ee0/SubjectMgr.h>
#include <blueprint/Node.h>
#include <blueprint/CompNode.h>

#include <painting2/RenderSystem.h>
#include <painting3/MaterialMgr.h>
#include <painting3/Blackboard.h>
#include <painting3/WindowContext.h>
#include <painting3/PerspCam.h>
#include <painting3/Shader.h>
#include <renderpipeline/HeightfieldGrayRenderer.h>
#include <tessellation/Painter.h>
#include <node0/SceneNode.h>
#include <node3/RenderSystem.h>
#include <citygraph/ParamImpl.h>

namespace
{

const float    NODE_RADIUS = 10;
const uint32_t LIGHT_SELECT_COLOR = 0x88000088;

}

namespace citylab
{

WxPreviewCanvas::WxPreviewCanvas(const ur::Device& dev, ee0::WxStagePage* stage,
                                 ECS_WORLD_PARAM const ee0::RenderContext& rc)
    : ee3::WxStageCanvas(dev, stage, ECS_WORLD_VAR &rc, nullptr, true)
{
    m_hf_rd = std::make_shared<rp::HeightfieldGrayRenderer>(dev);
}

WxPreviewCanvas::~WxPreviewCanvas()
{
    if (m_graph_page)
    {
        auto sub_mgr = m_graph_page->GetSubjectMgr();
        sub_mgr->UnregisterObserver(ee0::MSG_NODE_SELECTION_INSERT, this);
        sub_mgr->UnregisterObserver(ee0::MSG_NODE_SELECTION_CLEAR, this);
    }
}

void WxPreviewCanvas::OnNotify(uint32_t msg, const ee0::VariantSet& variants)
{
    ee3::WxStageCanvas::OnNotify(msg, variants);

	switch (msg)
	{
	case ee0::MSG_NODE_SELECTION_INSERT:
		OnSelectionInsert(variants);
		break;
    case ee0::MSG_NODE_SELECTION_CLEAR:
        OnSelectionClear(variants);
        break;
	}
}

void WxPreviewCanvas::SetGraphPage(const bp::WxGraphPage<citygraph::ParamType>* graph_page)
{
    m_graph_page = graph_page;

    auto sub_mgr = graph_page->GetSubjectMgr();
    sub_mgr->RegisterObserver(ee0::MSG_NODE_SELECTION_INSERT, this);
    sub_mgr->RegisterObserver(ee0::MSG_NODE_SELECTION_CLEAR, this);

    std::static_pointer_cast<ShortestPathOP>(m_ops[OP_SHORTEST_PATH])
        ->SetEditorSubMgr(m_graph_page->GetSubjectMgr());
}

void WxPreviewCanvas::InitEditOP(const ee0::EditOPPtr& default_op)
{
    m_ops[OP_DEFAULT] = default_op;
    m_ops[OP_SHORTEST_PATH] = std::make_shared<ShortestPathOP>(
        m_camera, GetViewport(), m_stage->GetSubjectMgr()
    );
}

void WxPreviewCanvas::DrawBackground3D() const
{
//    ee3::WxStageCanvas::DrawBackgroundGrids(10.0f, 0.2f);
//    ee3::WxStageCanvas::DrawBackgroundCross();
}

void WxPreviewCanvas::DrawForeground3D() const
{
    //auto& shaders = m_hf_rd->GetAllShaders();
    //if (!shaders.empty()) {
    //    assert(shaders.size() == 1);
    //    auto& wc = std::const_pointer_cast<pt3::WindowContext>(GetWidnowContext().wc3);
    //    if (shaders[0]->get_type() == rttr::type::get<pt3::Shader>()) {
    //        std::static_pointer_cast<pt3::Shader>(shaders[0])->AddNotify(wc);
    //    }
    //}

    pt0::RenderContext rc;
    rc.AddVar(
        pt3::MaterialMgr::PositionUniforms::light_pos.name,
        pt0::RenderVariant(sm::vec3(0, 2, -4))
    );
    if (m_camera->TypeID() == pt0::GetCamTypeID<pt3::PerspCam>())
    {
        auto persp = std::static_pointer_cast<pt3::PerspCam>(m_camera);
        rc.AddVar(
            pt3::MaterialMgr::PositionUniforms::cam_pos.name,
            pt0::RenderVariant(persp->GetPos())
        );
    }
    //auto& wc = pt3::Blackboard::Instance()->GetWindowContext();
    //assert(wc);
    //rc.AddVar(
    //    pt3::MaterialMgr::PosTransUniforms::view.name,
    //    pt0::RenderVariant(wc->GetViewMat())
    //);
    //rc.AddVar(
    //    pt3::MaterialMgr::PosTransUniforms::projection.name,
    //    pt0::RenderVariant(wc->GetProjMat())
    //);

    tess::Painter pt;

    auto cam_mat = m_camera->GetProjectionMat() * m_camera->GetViewMat();

    DrawSelected(pt, cam_mat, rc);

    ur::RenderState rs;
    pt2::RenderSystem::DrawPainter(m_dev, *GetRenderContext().ur_ctx, rs, pt);
}

void WxPreviewCanvas::DrawForeground2D() const
{
}

void WxPreviewCanvas::OnSelectionInsert(const ee0::VariantSet& variants)
{
    auto var_obj = variants.GetVariant("obj");
    GD_ASSERT(var_obj.m_type == ee0::VT_PVOID, "no var in vars: obj");
    const ee0::GameObj obj = *static_cast<const ee0::GameObj*>(var_obj.m_val.pv);
    GD_ASSERT(GAME_OBJ_VALID(obj), "err scene obj");

    m_selected = obj;

    SetupRenderer();

    auto node = GetSelectedNode();
    if (node)
    {
        auto type = node->get_type();
        if (type == rttr::type::get<node::ShortestPath>()) {
            std::static_pointer_cast<ShortestPathOP>(m_ops[OP_SHORTEST_PATH])
                ->Setup(std::static_pointer_cast<node::ShortestPath>(node), obj);
            m_stage->GetImpl().SetEditOP(m_ops[OP_SHORTEST_PATH]);
        } else {
            m_stage->GetImpl().SetEditOP(m_ops[OP_DEFAULT]);
        }
    }
}

void WxPreviewCanvas::OnSelectionClear(const ee0::VariantSet& variants)
{
    m_selected.reset();
    m_hf_rd->Clear();
}

void WxPreviewCanvas::DrawSelected(tess::Painter& pt, const sm::mat4& cam_mat,
                                   const pt0::RenderContext& rc) const
{
    auto op = GetSelectedOp();
    if (!op) {
        return;
    }

    auto& ctx = *GetRenderContext().ur_ctx;

    auto hf = op->GetHeightField();
    if (hf) {
        m_hf_rd->Draw(ctx);
    }

    auto& vals = op->GetAllValues();
    for (auto& v : vals)
    {
        switch (v->Type())
        {
        case citygraph::ParamType::Path:
        {
            auto& path = std::static_pointer_cast<citygraph::PathParam>(v)->GetPath();

            tess::Painter pt;
            pt.AddPolyline3D(path.data(), path.size(), [&](const sm::vec3& pos3)->sm::vec2 {
                return GetViewport().TransPosProj3ToProj2(pos3, cam_mat);
            }, 0xff0000ff);

            ur::RenderState rs;
            pt2::RenderSystem::DrawPainter(m_dev, ctx, rs, pt);
        }
            break;
        }
    }
}

void WxPreviewCanvas::SetupRenderer()
{
    auto op = GetSelectedOp();
    if (!op) {
        return;
    }

    auto hf = op->GetHeightField();
    if (hf) {
        m_hf_rd->Setup(m_dev, *GetRenderContext().ur_ctx, hf);
        SetDirty();
    }
}

bp::NodePtr WxPreviewCanvas::GetSelectedNode() const
{
    if (!m_selected || !m_selected->HasUniqueComp<bp::CompNode>()) {
        return nullptr;
    }

    auto eval = m_graph_page->GetSceneTree()->GetCurrEval();
    if (!eval) {
        return nullptr;
    }

    auto& cnode = m_selected->GetUniqueComp<bp::CompNode>();
    auto bp_node = cnode.GetNode();
    if (!bp_node) {
        return nullptr;
    }

    return cnode.GetNode();
}

citygraph::OperatorPtr WxPreviewCanvas::GetSelectedOp() const
{
    if (!m_graph_page) {
        return nullptr;
    }

    auto eval = m_graph_page->GetSceneTree()->GetCurrEval();
    if (!eval) {
        return nullptr;
    }

    auto front_node = GetSelectedNode();
    if (front_node) {
        return std::static_pointer_cast<citygraph::Operator>(eval->QueryBackNode(*front_node));
    } else {
        return nullptr;
    }
}

}
