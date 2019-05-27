#pragma once
#include "header.h"

class QeTransform : public QeComponent {
   public:
    QeTransform(QeObjectMangerKey &_key) : QeComponent(_key) {}
    ~QeTransform() {}

    // Face is Euler angles(pitch-roll-yaw)  roll, pitch, yaw.
    QeVector3f localPosition, localScale, localFaceEular;
    QeVector3f rotateSpeed, revoluteSpeed;
    bool revoluteFixAxisX = false, revoluteFixAxisY = false, revoluteFixAxisZ = false;
    int targetAnimationOID;
    const char *targetBoneName = nullptr;

    virtual void initialize(QeAssetXML *_property, QeObject *_owner);
    virtual void update1();

    QeVector3f worldPosition();
    QeVector3f worldScale();
    QeVector3f worldFaceEular();
    QeVector3f worldFaceVector();
    QeVector3f localFaceVector();

    void setWorldPosition(QeVector3f &_worldPosition);
    void setWorldScale(QeVector3f &_worldScale);
    void setWorldFaceByEular(QeVector3f &_worldFaceEular);
    void setWorldFaceByVector(QeVector3f &_worldFaceVector);

    void move(QeVector3f &_addMove, QeVector3f &_face, QeVector3f &_up);
    void revolute(QeVector3f &_addRevolute, QeVector3f &_centerPosition, bool bFixX = false, bool bFixY = false,
                  bool bFixZ = false);

    QeMatrix4x4f worldTransformMatrix(bool bRotate = true, bool bFixSize = false);
};