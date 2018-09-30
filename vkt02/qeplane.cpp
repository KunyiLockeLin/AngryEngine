#include "qeheader.h"


void QePlane::initialize(QeAssetXML* _property, QeObject* _owner) {

	QeComponent::initialize(_property, _owner);

	modelData = nullptr;
	materialData = nullptr;
	VK->createBuffer(modelBuffer, sizeof(bufferData), nullptr);

	AST->getXMLiValue((int*)&planeType, initProperty, 1, "planeType");
	AST->getXMLiValue(&materialOID, initProperty, 1, "materialOID");
	targetCameraOID = 0;
	AST->getXMLiValue(&targetCameraOID, initProperty, 1, "targetCameraOID");

	bUpdateTargetCameraOID = false;
	if (targetCameraOID) bUpdateTargetCameraOID = true;

	bUpdateMaterialOID = false;
	if (materialOID) bUpdateMaterialOID = true;
	graphicsPipeline.bAlpha = false;

	switch (planeType) {
	case ePlane_3D:
		shaderKey = "plane";
		bRotate = true;
		GRAP->models.push_back(this);
		break;
	case ePlane_billboard:
		shaderKey = "billboard";
		bRotate = false;
		GRAP->models.push_back(this);
		break;
	case ePlane_2D:
		shaderKey = "b2d";
		bRotate = false;
		GRAP->add2DModel(this);
		break;
	}

	AST->setGraphicsShader(graphicsShader, nullptr, shaderKey);

	VK->createDescriptorSet(descriptorSet);
	VK->updateDescriptorSet(&createDescriptorSetModel(), descriptorSet);
}

void QePlane::clear(){
	QeModel::clear();
	if (planeType == ePlane_2D)	eraseElementFromVector<QeModel*>(GRAP->models2D, this);
}

QeDataDescriptorSetModel QePlane::createDescriptorSetModel() {

	if (targetCameraOID) {
		QeCamera* camera = (QeCamera*)OBJMGR->findComponent(eComponent_camera, targetCameraOID);

		if (camera) {
			QeDataDescriptorSetModel descriptorSetData;
			descriptorSetData.modelBuffer = modelBuffer.buffer;
			QeDataRender* render = GRAP->getRender(eRender_color, targetCameraOID);

			if (render->subpass.size() > 0)
				descriptorSet.imageLayout = VK_IMAGE_LAYOUT_UNDEFINED;
			else
				descriptorSet.imageLayout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

			descriptorSetData.baseColorMapImageView = render->colorImage.view;
			descriptorSetData.baseColorMapSampler = render->colorImage.sampler;
			return descriptorSetData;
		}
	}
	return QeModel::createDescriptorSetModel();
}


void QePlane::update1() {

	if (bUpdateTargetCameraOID) {
		VK->updateDescriptorSet(&createDescriptorSetModel(), descriptorSet);
		bUpdateTargetCameraOID = true;

		QeAssetXML * node = AST->getXMLNode(4, AST->CONFIG, "shaders", "graphics", "render");
		AST->setGraphicsShader(graphicsShader, node, shaderKey);
	}

	if (targetCameraOID) {
		QeCamera* camera = (QeCamera*)OBJMGR->findComponent(eComponent_camera, targetCameraOID);

		if (camera) {
			QeVector3f scale = owner->transform->worldScale();
			scale.x *= MATH->fastSqrt((float(camera->renderSize.width) / camera->renderSize.height));
			bufferData.model = MATH->getTransformMatrix(owner->transform->worldPosition(), owner->transform->worldFaceEular(), scale);
			VK->setMemoryBuffer(modelBuffer, sizeof(bufferData), &bufferData);
		}
	}
	else {
		QeModel::update1();
	}
}