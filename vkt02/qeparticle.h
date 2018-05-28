#pragma once
#include "qeheader.h"

class QeParticle : public QeBillboard
{
public:

	QeParticle(QeObjectMangerKey& _key) :QeBillboard(_key) {}
	~QeParticle() {}

	VkShaderModule computeShader = VK_NULL_HANDLE;
	QeAssetShader shader;
	//VkCommandBuffer commandBuffer = VK_NULL_HANDLE;

	uint32_t  PARTICLES_COUNT = 100;
	std::vector<QeVertex> particles;
	QeVKBuffer		VertexBuffer;

	QeTimer timer;

	virtual void init(QeAssetXML* _property);
	virtual void createPipeline();
	virtual void updateDrawCommandBuffer(VkCommandBuffer& drawCommandBuffer);
	virtual void updateComputeCommandBuffer(VkCommandBuffer& computeCommandBuffer);
	virtual void updateCompute(float time);
};
