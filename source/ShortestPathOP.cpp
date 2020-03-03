#include "citylab/ShortestPathOP.h"
#include "citylab/RegistNodes.h"

#include <ee0/MsgHelper.h>
#include <ee0/SubjectMgr.h>
#include <blueprint/MessageID.h>

#include <SM_Ray.h>
#include <SM_RayIntersect.h>
#include <tessellation/Painter.h>
#include <painting2/RenderSystem.h>
#include <painting3/Viewport.h>
#include <painting3/PerspCam.h>

namespace citylab
{

ShortestPathOP::ShortestPathOP(const std::shared_ptr<pt0::Camera>& camera,
                               const pt3::Viewport& vp, const ee0::SubjectMgrPtr& sub_mgr)
    : ee3::WorldTravelOP(camera, vp, sub_mgr, true)
    , m_vp(vp)
    , m_sub_mgr(sub_mgr)
{
    m_start_pos.MakeInvalid();
    m_end_pos.MakeInvalid();
}

bool ShortestPathOP::OnMouseLeftDown(int x, int y)
{
    if (ee3::WorldTravelOP::OnMouseLeftDown(x, y)) {
        return true;
    }

    CalcMousePos(m_start_pos, sm::ivec2(x, y));

    return false;
}

bool ShortestPathOP::OnMouseLeftUp(int x, int y)
{
    if (ee3::WorldTravelOP::OnMouseLeftUp(x, y)) {
        return true;
    }

    if (!m_node || !m_start_pos.IsValid() || !m_end_pos.IsValid()) {
        return false;
    }

    m_node->m_start_pos = m_start_pos;
    m_node->m_end_pos = m_end_pos;

    ee0::MsgHelper::SendObjMsg(*m_editor_sub_mgr,
        m_editor_node, bp::MSG_BP_NODE_PROP_CHANGED);
    m_sub_mgr->NotifyObservers(ee0::MSG_SET_CANVAS_DIRTY);

    m_start_pos.MakeInvalid();
    m_end_pos.MakeInvalid();

    return false;
}

bool ShortestPathOP::OnMouseDrag(int x, int y)
{
    if (ee3::WorldTravelOP::OnMouseDrag(x, y)) {
        return true;
    }

    CalcMousePos(m_end_pos, sm::ivec2(x, y));

    return false;
}

bool ShortestPathOP::OnDraw() const
{
    if (ee3::WorldTravelOP::OnDraw()) {
        return true;
    }

    if (m_start_pos.IsValid() && m_end_pos.IsValid())
    {
        auto cam_mat = m_camera->GetProjectionMat() * m_camera->GetViewMat();

        tess::Painter pt;
        pt.AddLine3D(m_start_pos, m_end_pos, [&](const sm::vec3& pos3)->sm::vec2 {
            return m_vp.TransPosProj3ToProj2(pos3, cam_mat);
        }, 0xff0000ff);
        pt2::RenderSystem::DrawPainter(pt);
    }

    return false;
}

bool ShortestPathOP::CalcMousePos(sm::vec3& proj_pos, const sm::ivec2& screen_pos) const
{
    auto cam_type = m_camera->TypeID();
    if (cam_type != pt0::GetCamTypeID<pt3::PerspCam>()) {
        return false;
    }

    auto p_cam = std::dynamic_pointer_cast<pt3::PerspCam>(m_camera);
    sm::vec3 ray_dir = m_vp.TransPos3ScreenToDir(
        sm::vec2(static_cast<float>(screen_pos.x), static_cast<float>(screen_pos.y)), *p_cam);
    sm::Ray ray(p_cam->GetPos(), ray_dir);

    sm::Plane plane(sm::vec3(0, 1, 0), sm::vec3(0, 0, 0));
    sm::vec3 cross;
    if (!sm::ray_plane_intersect(ray, plane, &cross)) {
        return false;
    }

    proj_pos = cross;

    return true;
}

}