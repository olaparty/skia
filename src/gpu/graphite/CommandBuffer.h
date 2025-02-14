/*
 * Copyright 2021 Google LLC
 *
 * Use of this source code is governed by a BSD-style license that can be
 * found in the LICENSE file.
 */

#ifndef skgpu_graphite_CommandBuffer_DEFINED
#define skgpu_graphite_CommandBuffer_DEFINED

#include "include/core/SkColor.h"
#include "include/core/SkRect.h"
#include "include/core/SkRefCnt.h"
#include "include/private/SkTArray.h"
#include "src/gpu/graphite/AttachmentTypes.h"
#include "src/gpu/graphite/CommandTypes.h"
#include "src/gpu/graphite/DrawTypes.h"
#include "src/gpu/graphite/DrawWriter.h"

namespace skgpu {
class RefCntedCallback;
}

namespace skgpu::graphite {
class Buffer;
class Gpu;
class GraphicsPipeline;
class Resource;
class Sampler;
class Texture;
class TextureProxy;


class CommandBuffer : public SkRefCnt, private DrawDispatcher {
public:
    ~CommandBuffer() override;

#ifdef SK_DEBUG
    bool hasWork() { return fHasWork; }
#endif

    void trackResource(sk_sp<Resource> resource);
    // Release all tracked Resources
    void releaseResources();

    void addFinishedProc(sk_sp<RefCntedCallback> finishedProc);
    void callFinishedProcs(bool success);

    bool beginRenderPass(const RenderPassDesc&,
                         sk_sp<Texture> colorTexture,
                         sk_sp<Texture> resolveTexture,
                         sk_sp<Texture> depthStencilTexture);
    virtual void endRenderPass() = 0;

    //---------------------------------------------------------------
    // Can only be used within renderpasses
    //---------------------------------------------------------------
    void bindGraphicsPipeline(sk_sp<GraphicsPipeline> graphicsPipeline);
    void bindUniformBuffer(UniformSlot, sk_sp<Buffer>, size_t bufferOffset);

    void bindDrawBuffers(BindBufferInfo vertices,
                         BindBufferInfo instances,
                         BindBufferInfo indices) final;

    void bindTextureAndSampler(sk_sp<Texture>, sk_sp<Sampler>, int bindIndex);

    // TODO: do we want to handle multiple scissor rects and viewports?
    void setScissor(unsigned int left, unsigned int top, unsigned int width, unsigned int height) {
        this->onSetScissor(left, top, width, height);
    }

    void setViewport(float x, float y, float width, float height,
                     float minDepth = 0, float maxDepth = 1) {
        this->onSetViewport(x, y, width, height, minDepth, maxDepth);
    }

    void setBlendConstants(std::array<float, 4> blendConstants) {
        this->onSetBlendConstants(blendConstants);
    }

    void draw(PrimitiveType type, unsigned int baseVertex, unsigned int vertexCount) final {
        this->onDraw(type, baseVertex, vertexCount);
        SkDEBUGCODE(fHasWork = true;)
    }
    void drawIndexed(PrimitiveType type, unsigned int baseIndex, unsigned int indexCount,
                     unsigned int baseVertex) final {
        this->onDrawIndexed(type, baseIndex, indexCount, baseVertex);
        SkDEBUGCODE(fHasWork = true;)
    }
    void drawInstanced(PrimitiveType type, unsigned int baseVertex, unsigned int vertexCount,
                       unsigned int baseInstance, unsigned int instanceCount) final {
        this->onDrawInstanced(type, baseVertex, vertexCount, baseInstance, instanceCount);
        SkDEBUGCODE(fHasWork = true;)
    }
    void drawIndexedInstanced(PrimitiveType type, unsigned int baseIndex, unsigned int indexCount,
                              unsigned int baseVertex, unsigned int baseInstance,
                              unsigned int instanceCount) final {
        this->onDrawIndexedInstanced(type, baseIndex, indexCount, baseVertex, baseInstance,
                                     instanceCount);
        SkDEBUGCODE(fHasWork = true;)
    }

    // When using a DrawWriter dispatching directly to a CommandBuffer, binding of pipelines and
    // uniforms must be coordinated with forNewPipeline() and forDynamicStateChange(). The direct
    // draw calls and vertex buffer binding calls on CB should not be intermingled with the writer.
    DrawDispatcher* asDrawDispatcher() { return this; }

    //---------------------------------------------------------------
    // Can only be used outside renderpasses
    //---------------------------------------------------------------
    bool copyTextureToBuffer(sk_sp<Texture>,
                             SkIRect srcRect,
                             sk_sp<Buffer>,
                             size_t bufferOffset,
                             size_t bufferRowBytes);
    bool copyBufferToTexture(const Buffer*,
                             sk_sp<Texture>,
                             const BufferTextureCopyData*,
                             int count);

protected:
    CommandBuffer();

private:
    // TODO: Once all buffer use goes through the DrawBufferManager, we likely do not need to track
    // refs every time a buffer is bound, since the DBM will transfer ownership for any used buffer
    // to the CommandBuffer.
    void bindVertexBuffers(sk_sp<Buffer> vertexBuffer, size_t vertexOffset,
                           sk_sp<Buffer> instanceBuffer, size_t instanceOffset);
    void bindIndexBuffer(sk_sp<Buffer> indexBuffer, size_t bufferOffset);

    virtual bool onBeginRenderPass(const RenderPassDesc&,
                                   const Texture* colorTexture,
                                   const Texture* resolveTexture,
                                   const Texture* depthStencilTexture) = 0;

    virtual void onBindGraphicsPipeline(const GraphicsPipeline*) = 0;
    virtual void onBindUniformBuffer(UniformSlot, const Buffer*, size_t bufferOffset) = 0;
    virtual void onBindVertexBuffers(const Buffer* vertexBuffer, size_t vertexOffset,
                                     const Buffer* instanceBuffer, size_t instanceOffset) = 0;
    virtual void onBindIndexBuffer(const Buffer* indexBuffer, size_t bufferOffset) = 0;

    virtual void onBindTextureAndSampler(sk_sp<Texture>,
                                         sk_sp<Sampler>,
                                         unsigned int bindIndex) = 0;

    virtual void onSetScissor(unsigned int left, unsigned int top,
                              unsigned int width, unsigned int height) = 0;
    virtual void onSetViewport(float x, float y, float width, float height,
                               float minDepth, float maxDepth) = 0;
    virtual void onSetBlendConstants(std::array<float, 4> blendConstants) = 0;

    virtual void onDraw(PrimitiveType type, unsigned int baseVertex, unsigned int vertexCount) = 0;
    virtual void onDrawIndexed(PrimitiveType type, unsigned int baseIndex, unsigned int indexCount,
                               unsigned int baseVertex) = 0;
    virtual void onDrawInstanced(PrimitiveType type,
                                 unsigned int baseVertex, unsigned int vertexCount,
                                 unsigned int baseInstance, unsigned int instanceCount) = 0;
    virtual void onDrawIndexedInstanced(PrimitiveType type, unsigned int baseIndex,
                                        unsigned int indexCount, unsigned int baseVertex,
                                        unsigned int baseInstance, unsigned int instanceCount) = 0;

    virtual bool onCopyTextureToBuffer(const Texture*,
                                       SkIRect srcRect,
                                       const Buffer*,
                                       size_t bufferOffset,
                                       size_t bufferRowBytes) = 0;
    virtual bool onCopyBufferToTexture(const Buffer*,
                                       const Texture*,
                                       const BufferTextureCopyData*,
                                       int count) = 0;

#ifdef SK_DEBUG
    bool fHasWork = false;
#endif

    inline static constexpr int kInitialTrackedResourcesCount = 32;
    SkSTArray<kInitialTrackedResourcesCount, sk_sp<Resource>> fTrackedResources;
    SkTArray<sk_sp<RefCntedCallback>> fFinishedProcs;
};

} // namespace skgpu::graphite

#endif // skgpu_graphite_CommandBuffer_DEFINED
