#pragma once

#include <ee3/WorldTravelOP.h>

#include <SM_Vector.h>
#include <node0/typedef.h>

#include <vector>

namespace citylab
{

namespace node { class ShortestPath; }

class ShortestPathOP : public ee3::WorldTravelOP
{
public:
    ShortestPathOP(const std::shared_ptr<pt0::Camera>& camera,
        const pt3::Viewport& vp, const ee0::SubjectMgrPtr& sub_mgr);

    virtual bool OnMouseLeftDown(int x, int y) override;
    virtual bool OnMouseLeftUp(int x, int y) override;
    virtual bool OnMouseDrag(int x, int y) override;

    virtual bool OnDraw(const ur2::Device& dev, ur2::Context& ctx) const;

    void Setup(const std::shared_ptr<node::ShortestPath>& node,
        const n0::SceneNodePtr& editor_node)
    {
        m_node = node;
        m_editor_node = editor_node;
    }

    void SetEditorSubMgr(const ee0::SubjectMgrPtr& editor_sub_mgr) {
        m_editor_sub_mgr = editor_sub_mgr;
    }

private:
    bool CalcMousePos(sm::vec3& proj_pos, const sm::ivec2& screen_pos) const;

private:
    const pt3::Viewport& m_vp;
    ee0::SubjectMgrPtr m_sub_mgr = nullptr;

    ee0::SubjectMgrPtr m_editor_sub_mgr;

    std::shared_ptr<node::ShortestPath> m_node = nullptr;
    n0::SceneNodePtr m_editor_node = nullptr;

    sm::vec3 m_start_pos, m_end_pos;

}; // ShortestPathOP

}