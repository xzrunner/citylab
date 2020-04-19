#pragma once

#include <ee3/WxStageCanvas.h>
#include <blueprint/WxGraphPage.h>

#include <citygraph/ParamType.h>
#include <citygraph/typedef.h>

namespace rp { class HeightfieldRenderer; }

namespace citylab
{

class WxEditorPanel;

class WxPreviewCanvas : public ee3::WxStageCanvas
{
public:
    WxPreviewCanvas(const ur2::Device& dev, ee0::WxStagePage* stage,
        ECS_WORLD_PARAM const ee0::RenderContext& rc);
    virtual ~WxPreviewCanvas();

    virtual void OnNotify(uint32_t msg, const ee0::VariantSet& variants) override;

    void SetGraphPage(const bp::WxGraphPage<citygraph::ParamType>* graph_page);

    void InitEditOP(const ee0::EditOPPtr& default_op);

protected:
    virtual void DrawBackground3D() const override;
    virtual void DrawForeground3D() const override;
    virtual void DrawForeground2D() const override;

private:
    void OnSelectionInsert(const ee0::VariantSet& variants);
    void OnSelectionClear(const ee0::VariantSet& variants);

    void DrawSelected(tess::Painter& pt, const sm::mat4& cam_mat,
        const pt0::RenderContext& rc) const;

    void SetupRenderer();

    bp::NodePtr GetSelectedNode() const;
    citygraph::OperatorPtr GetSelectedOp() const;

private:
    enum OperatorID
    {
        OP_DEFAULT,
        OP_SHORTEST_PATH,

        OP_MAX_NUM
    };

private:
    const bp::WxGraphPage<citygraph::ParamType>* m_graph_page = nullptr;

    std::shared_ptr<rp::HeightfieldRenderer> m_hf_rd = nullptr;

    n0::SceneNodePtr m_selected = nullptr;

    std::array<ee0::EditOPPtr, OP_MAX_NUM> m_ops;

}; // WxPreviewCanvas

}