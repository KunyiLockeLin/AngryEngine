#pragma once

#define DllExport __declspec(dllexport)
#define DllImport __declspec(dllimport)

#include <cstring>
#include <vector>
#include <map>
#include <chrono>
#include <fstream>
#include <random>
#include <iostream>

#define CASE_STR(r) \
    case r:         \
        return #r

#define SINGLETON_CLASS(class_name) \
   private:                         \
    class_name();                   \
                                    \
   public:                          \
    ~class_name();                  \
    static class_name &getInstance()

#define SINGLETON_INSTANCE(class_name)      \
    class_name &class_name::getInstance() { \
        static class_name instance;         \
        return instance;                    \
    }

#define MANAGER_KEY_CLASS(class_name)            \
    class DllExport Ae##class_name##ManagerKey { \
        friend class Ae##class_name##Manager;    \
                                                 \
       private:                                  \
        Ae##class_name##ManagerKey() {}          \
    }

#define MANAGER_KEY_INSTANCE(class_name) \
   private:                              \
    Ae##class_name##ManagerKey key

#define REQUIRED_MANAGER_KEY(class_name, manager) \
   public:                                        \
    AeUIComponent##class_name(const Ae##manager##ManagerKey &key)
/*
#define COMPONENT_CLASS_DECLARE(component_type, manager)                         \
    AeUIComponent##component_type(Ae##manager##ManagerKey &_key) : AeUIComponentBase(_key) {} \
    ~AeUIComponent##component_type() {}                                            \
    AeUIObjectDataComponent##component_type component_data;           \
    virtual void initialize(AeXMLNode *_property, QeObject *_owner);

#define COMPONENT_CLASS_DECLARE_PARENT(component_type, parent_component_type)         \
    Ae##component_type(AeObjectManagerKey &_key) : Qe##parent_component_type(_key) {} \
    ~Qe##component_type() {}                                                          \
    AeGameObjectDataComponent##component_type component_data;                         \
    virtual void initialize(AeXMLNode *_property, QeObject *_owner);

#define COMPONENT_CLASS_DECLARE_INITIALIZE(component_type, ...)                      \
    Qe##component_type(AeObjectManagerKey &_key) : QeComponent(_key), __VA_ARGS__ {} \
    ~Qe##component_type() {}                                                         \
    AeGameObjectDataComponent##component_type component_data;                        \
    virtual void initialize(AeXMLNode *_property, QeObject *_owner);

#define COMPONENT_CLASS_DECLARE_PARENT_INITIALIZE(component_type, parent_component_type, ...)      \
    Qe##component_type(AeObjectManagerKey &_key) : Qe##parent_component_type(_key), __VA_ARGS__ {} \
    ~Qe##component_type() {}                                                                       \
    AeGameObjectDataComponent##component_type component_data;                                      \
    virtual void initialize(AeXMLNode *_property, QeObject *_owner);

#define COMPONENT_INITIALIZE                    \
    QeComponent::initialize(_property, _owner); \
    component_data.read(*_property);

#define COMPONENT_INITIALIZE_PARENT(parent_component_type)    \
    Qe##parent_component_type::initialize(_property, _owner); \
    component_data.read(*_property); \
*/

#define BEGIN_NAMESPACE(x) namespace x {
#define END_NAMESPACE(x) }

#define SINGLETON_OBJECT(class_name)    \
   private:                             \
    class_name();                       \
                                        \
   public:                              \
    static class_name &get_instance() { \
        static class_name instance;     \
        return instance;                \
    }

#define OBJECT_KEY(class_name, friend_name) \
    class DllExport class_name {            \
        friend class friend_name;           \
                                            \
       private:                             \
        class_name() {}                     \
    };

#define MANAGED_OBJECT(class_name, key_name) \
   public:                                   \
    class_name(const key_name &key);         \
    static std::shared_ptr<class_name> create(const key_name &key) { return std::make_shared<class_name>(key); }

// template class DllExport std::vector<std::string>;
// template class DllExport std::basic_string<char>;

const char INDEX_NONE = -1;

#define ARRAY_SIZE(c_array) sizeof c_array / sizeof c_array[0]
// struct Empty {};
// typename std::conditional<N >= 1, T, Empty>::type x;
template <class T, int N>
struct AeArrayBase {
    T elements[N];
};

template <class T>
struct AeArrayBase<T, 2> {
    union {
        T elements[2];
        struct {
            T x, y;
        };
        struct {
            T u, v;
        };
        struct {
            T width, height;
        };
    };
};

template <class T>
struct AeArrayBase<T, 3> {
    union {
        T elements[3];
        struct {
            T x, y, z;
        };
        struct {
            T r, g, b;
        };
        struct {
            T width, height, depth;
        };
    };
};

template <class T>
struct AeArrayBase<T, 4> {
    union {
        T elements[4];
        struct {
            T x, y, z, w;
        };
        struct {
            T r, g, b, a;
        };
        struct {
            T width, height, depth, time;
        };
    };
};

template <class T, int N>
struct DllExport AeArray : public AeArrayBase<T, N> {
    AeArray();
    AeArray(std::initializer_list<T> l);
    template <class T2, int N2>
    AeArray(const AeArray<T2, N2> &other);
    template <class T2, int N2>
    AeArray(const AeArray<T2, N2> &other, T value);

    template <class T2, int N2>
    bool operator==(const AeArray<T2, N2> &other) const;

    template <class T2, int N2>
    bool operator!=(const AeArray<T2, N2> &other) const;

    template <class T2, int N2>
    AeArray<T, N> &operator=(const AeArray<T2, N2> &other);
    T &operator[](int index);

    template <class T2, int N2>
    AeArray<T, N> &operator+=(const AeArray<T2, N2> &other);
    template <class T2, int N2>
    AeArray<T, N> &operator-=(const AeArray<T2, N2> &other);
    template <class T2, int N2>
    AeArray<T, N> &operator*=(const AeArray<T2, N2> &other);
    template <class T2, int N2>
    AeArray<T, N> &operator/=(const AeArray<T2, N2> &other);

    template <class T2, int N2>
    AeArray<T, N> operator+(const AeArray<T2, N2> &other);
    template <class T2, int N2>
    AeArray<T, N> operator-(const AeArray<T2, N2> &other);
    template <class T2, int N2>
    AeArray<T, N> operator*(const AeArray<T2, N2> &other);
    template <class T2, int N2>
    AeArray<T, N> operator/(const AeArray<T2, N2> &other);

    template <class T2>
    AeArray<T, N> &operator=(const T2 &other);
    template <class T2>
    AeArray<T, N> &operator+=(const T2 &other);
    template <class T2>
    AeArray<T, N> &operator-=(const T2 &other);
    template <class T2>
    AeArray<T, N> &operator*=(const T2 &other);
    template <class T2>
    AeArray<T, N> &operator/=(const T2 &other);

    template <class T2>
    AeArray<T, N> operator+(const T2 &other);
    template <class T2>
    AeArray<T, N> operator-(const T2 &other);
    template <class T2>
    AeArray<T, N> operator*(const T2 &other);
    template <class T2>
    AeArray<T, N> operator/(const T2 &other);
};
/*
template <class T>
struct DllExport AeArray<T, 2> {
    union {
        T elements[2];
        struct {
            T x, y;
        };
        struct {
            T width, height;
        };
        struct {
            T u, v;
        };
    };
};
*/

/*
template <class T, int N>
struct DllExport AeMatrix {
    const int size = N;
    T e[N][N];

    AeMatrix();
    AeMatrix(float value);
    AeMatrix<T, N> &operator*=(const AeMatrix<T, N> &other);
    AeMatrix<T, N> operator*(const AeMatrix<T, N> &other);
    AeArray<N> operator*(const AeArray<N> &other);
    AeMatrix<T, N> &operator/=(const float &other);
};
*/
/*
struct DllExport QeVector2i {
    int x, y;

    QeVector2i();
    QeVector2i(int _x, int _y);
};

struct DllExport QeVector2f {
    float x, y;

    QeVector2f();
    QeVector2f(float _x, float _y);

    bool operator==(const QeVector2f &other) const;
    QeVector2f &operator+=(const QeVector2f &other);
    QeVector2f &operator-=(const QeVector2f &other);
    QeVector2f &operator/=(const float &other);
    QeVector2f &operator*=(const float &other);
    QeVector2f operator/(const float &other);
    QeVector2f operator*(const float &other);
};

struct DllExport QeVector3i {
    int x, y, z;

    QeVector3i();
    QeVector3i(int _x, int _y, int _z);

    bool operator==(const QeVector3i &other) const;
    QeVector3i &operator+=(const QeVector3i &other);
    QeVector3i &operator-=(const QeVector3i &other);
    QeVector3i &operator-=(const int &other);
    QeVector3i &operator/=(const int &other);
    QeVector3i operator-(const QeVector3i &other);
    QeVector3i operator/(const int &other);
    QeVector3i operator*(const int &other);
};

struct QeVector4f;
struct DllExport QeVector3f {
    float x, y, z;

    QeVector3f();
    QeVector3f(float _x, float _y, float _z);
    QeVector3f(int _x, int _y, int _z);
    QeVector3f(const QeVector4f &other);

    bool operator==(const QeVector3f &other) const;
    bool operator!=(const QeVector3f &other) const;

    QeVector3f &operator=(const QeVector4f &other);
    QeVector3f &operator+=(const QeVector3f &other);
    QeVector3f &operator-=(const QeVector3f &other);
    QeVector3f &operator*=(const QeVector3f &other);
    QeVector3f &operator/=(const QeVector3f &other);
    QeVector3f &operator-=(const float &other);
    QeVector3f &operator+=(const float &other);
    QeVector3f &operator/=(const float &other);
    QeVector3f &operator*=(const float &other);
    QeVector3f operator+(const QeVector3f &other);
    QeVector3f operator-(const QeVector3f &other);
    QeVector3f operator*(const QeVector3f &other);
    QeVector3f operator+(const float &other);
    QeVector3f operator/(const float &other);
    QeVector3f operator*(const float &other);
};

struct DllExport QeVector4s {
    short int x, y, z, w;
    QeVector4s();
};

struct DllExport QeVector4i {
    int x, y, z, w;
    QeVector4i();
    QeVector4i &operator=(const QeVector4s &other);
};

struct DllExport QeVector4f {
    float x, y, z, w;

    QeVector4f();
    QeVector4f(float _x, float _y, float _z, float _w);
    QeVector4f(int _x, int _y, int _z, int _w);
    QeVector4f(const QeVector3f &other, float _w);

    bool operator==(const QeVector4f &other) const;
    bool operator!=(const QeVector4f &other) const;

    QeVector4f &operator=(const QeVector3f &other);
    QeVector4f &operator=(const QeVector2f &other);
    QeVector4f &operator=(const QeVector4s &other);
    QeVector4f &operator+=(const QeVector3f &other);
    QeVector4f operator+(const QeVector3f &other);
    QeVector4f operator-(const QeVector3f &other);
    QeVector4f operator/(const float &other);
    QeVector4f &operator*=(const float &other);
};
*/

struct DllExport QeMatrix4x4f {
    float _00, _01, _02, _03;
    float _10, _11, _12, _13;
    float _20, _21, _22, _23;
    float _30, _31, _32, _33;

    QeMatrix4x4f();
    QeMatrix4x4f(float _num);
    QeMatrix4x4f(float __00, float __01, float __02, float __03, float __10, float __11, float __12, float __13, float __20,
                 float __21, float __22, float __23, float __30, float __31, float __32, float __33);
    QeMatrix4x4f &operator*=(const QeMatrix4x4f &other);
    QeMatrix4x4f operator*(const QeMatrix4x4f &other);
    AeArray<float, 4> operator*(const AeArray<float, 4> &other);
    QeMatrix4x4f &operator/=(const float &other);
};

struct DllExport QeRay {
    AeArray<float, 3> origin;
    AeArray<float, 3> direction;
    AeArray<float, 3> positionByTime(float t);
};

struct DllExport QeRayHitRecord {
    float t;
    AeArray<float, 3> position;
    AeArray<float, 3> normal;
};

struct DllExport QeBoundingSphere {
    AeArray<float, 3> center;
    float radius;
};

struct DllExport QeBoundingBox {
    AeArray<float, 3> min;
    AeArray<float, 3> max;
};

class DllExport QeBinaryTree {
    void *data;
    int key;
    QeBinaryTree *left = nullptr;
    QeBinaryTree *right = nullptr;
    void insertNode(QeBinaryTree &node) {}
    void removeNode(QeBinaryTree &node) {}
    QeBinaryTree *getNode(int &key) { return nullptr; }
};

// Right-handed Coordinate System
class DllExport AeMath {
    SINGLETON_CLASS(AeMath);

    const float PI = 3.1415927f;
    const float RADIANS_TO_DEGREES = 180.0f / PI;
    const float DEGREES_TO_RADIANS = PI / 180;

    // template <class T>
    // T random(T start, T range);
    template <class T>
    T random(T start, T range);
    template <class T, int N>
    AeArray<T, N> randoms(T start, T range);
    template <int N>
    float dot(AeArray<float, N> &vec1, AeArray<float, N> &vec2);
    template <int N>
    float length(AeArray<float, N> &vec);
    template <int N>
    AeArray<float, N> normalize(AeArray<float, N> &_vec);

    QeMatrix4x4f lookAt(AeArray<float, 3> &_pos, AeArray<float, 3> &_center, AeArray<float, 3> &_up);
    QeMatrix4x4f perspective(float _fov, float _aspect, float _near, float _far);
    QeMatrix4x4f translate(AeArray<float, 3> &_pos);
    AeArray<float, 3> move(AeArray<float, 3> &_position, AeArray<float, 3> &_addMove, AeArray<float, 3> &_face,
                           AeArray<float, 3> &_up);
    QeMatrix4x4f rotate_quaternion(AeArray<float, 3> &_eulerAngles);
    QeMatrix4x4f rotate_quaternion(AeArray<float, 4> &quaternion);
    QeMatrix4x4f rotate_quaternion(float _angle, AeArray<float, 3> &_axis);
    AeArray<float, 4> eulerAngles_to_quaternion(AeArray<float, 3> &_eulerAngles);
    AeArray<float, 4> axis_to_quaternion(float _angle, AeArray<float, 3> &_axis);
    QeMatrix4x4f rotate_eularAngles(AeArray<float, 3> &_eulerAngles);  // (roll, pitch, yaw) or (bank, attitude, heading)
    QeMatrix4x4f rotate_axis(float _angle, AeArray<float, 3> &_axis);
    QeMatrix4x4f rotateX(float _angle);
    QeMatrix4x4f rotateY(float _angle);
    QeMatrix4x4f rotateZ(float _angle);
    AeArray<float, 4> matrix_to_quaternion(QeMatrix4x4f matrix);
    QeMatrix4x4f scale(AeArray<float, 3> &_size);
    QeMatrix4x4f transform(AeArray<float, 3> &_tanslation, AeArray<float, 4> &_rotation_quaternion, AeArray<float, 3> &_scale);
    QeMatrix4x4f getTransformMatrix(AeArray<float, 3> &_translate, AeArray<float, 3> &_rotateEuler, AeArray<float, 3> &_scale,
                                    AeArray<float, 3> &camera_world_position, bool bRotate = true, bool bFixSize = false);

    AeArray<float, 3> eulerAnglesToVector(AeArray<float, 3> &_eulerAngles);
    AeArray<float, 3> vectorToEulerAngles(AeArray<float, 3> &_vector);
    AeArray<float, 3> cross(AeArray<float, 3> &_vec1, AeArray<float, 3> &_vec2);
    float fastSqrt(float _number);
    bool inverse(QeMatrix4x4f &_inMat, QeMatrix4x4f &_outMat);
    QeMatrix4x4f transpose(QeMatrix4x4f &_mat);
    int clamp(int in, int low, int high);
    float clamp(float in, float low, float high);
    AeArray<float, 4> interpolateDir(AeArray<float, 4> &a, AeArray<float, 4> &b, float blend);
    AeArray<float, 3> interpolatePos(AeArray<float, 3> &start, AeArray<float, 3> &end, float progression);
    float getAnglefromVectors(AeArray<float, 3> &v1, AeArray<float, 3> &v2);
    AeArray<float, 3> revolute_axis(AeArray<float, 3> &_position, AeArray<float, 3> &_addRevolute,
                                    AeArray<float, 3> &_centerPosition, bool bFixX = false, bool bFixY = false, bool bFixZ = false);
    AeArray<float, 3> revolute_eularAngles(AeArray<float, 3> &_position, AeArray<float, 3> &_addRevolute,
                                           AeArray<float, 3> &_centerPosition, bool bFixX, bool bFixY, bool bFixZ);

    // void getAnglefromVector(QeVector3f& inV, float & outPolarAngle, float & outAzimuthalAngle);
    // void rotatefromCenter(QeVector3f& center, QeVector3f& pos, float polarAngle, float azimuthalAngle);
    // void rotatefromCenter(QeVector3f& center, QeVector3f& pos, QeVector2f & axis, float angle, bool bStopTop);
    bool hit_test_raycast_sphere(QeRay &ray, QeBoundingSphere &sphere, float maxDistance = 0.f, QeRayHitRecord *hit = nullptr);
    void quicksort(float *data, int count);
};
#define MATH AeMath::getInstance()

enum DllExport QeAssetType {
    eAssetXML = 0,
    eAssetJSON = 1,
};

struct AeJSONNode;
struct AeXMLNode;

MANAGER_KEY_CLASS(Common);
class DllExport AeCommonManager {
    MANAGER_KEY_INSTANCE(Common);
    SINGLETON_CLASS(AeCommonManager);

    AeJSONNode *getJSON(const char *_filePath);
    AeXMLNode *getXML(const char *_filePath);
    void removeXML(std::string path);

    std::vector<char> loadFile(const char *_filePath);
};
#define COM_MGR AeCommonManager::getInstance()

struct AeNode {
    std::string key;
    std::string value;
};

struct AeXMLNode;
struct AeXMLData {
    std::string version;
    std::vector<std::string> comments;
    std::string key;
    std::string value;
    std::vector<AeNode> elements;
    std::vector<AeXMLNode *> nexts;
    AeXMLNode *parent = nullptr;
};

struct DllExport AeXMLNode {
    AeXMLData *data;

    AeXMLNode();
    ~AeXMLNode();

    AeXMLNode *getXMLNode(const char *key);
    AeXMLNode *getXMLNode(std::vector<std::string> &keys);

    template <class T>
    T getXMLValue(const char *key);
    template <class T>
    AeXMLNode *getXMLValue(T &value, const char *key);

    template <class T, int N>
    AeArray<T, N> getXMLValues(const char *key);
    template <class T, int N>
    AeXMLNode *getXMLValues(AeArray<T, N> &value, const char *key);

    AeXMLNode *copyXMLNode();
    void copyXMLValue(AeXMLNode *to);
    void copyXMLNode(AeXMLNode *to);

    void addXMLNode(AeXMLNode *node);
    void removeXMLNode(AeXMLNode *node);

    void setXMLKey(const char *key);
    void setXMLValue(const char *value);
    void setXMLValue(const char *key, const char *value);

    void outputXML(const char *path, int level = 0, std::string *content = nullptr);
};

struct AeJSONNode;
struct AeJSON {
    std::vector<std::string> eKeysforValues;
    std::vector<std::string> eValues;
    std::vector<std::string> eKeysforNodes;
    std::vector<AeJSONNode *> eNodes;
    std::vector<std::string> eKeysforArrayValues;
    std::vector<std::vector<std::string>> eArrayValues;
    std::vector<std::string> eKeysforArrayNodes;
    std::vector<std::vector<AeJSONNode *>> eArrayNodes;
};

struct DllExport AeJSONNode {
    AeJSON *data;

    AeJSONNode();
    ~AeJSONNode();

    const char *getJSONValue(int length, ...);
    const char *getJSONValue(const char *keys[], int length);
    AeJSONNode *getJSONNode(int length, ...);
    AeJSONNode *getJSONNode(const char *keys[], int length);
    std::vector<std::string> *getJSONArrayValues(int length, ...);
    std::vector<std::string> *getJSONArrayValues(const char *keys[], int length);
    std::vector<AeJSONNode *> *getJSONArrayNodes(int length, ...);
    std::vector<AeJSONNode *> *getJSONArrayNodes(const char *keys[], int length);
    bool getJSONbValue(bool *output, int length, ...);
    bool getJSONiValue(int *output, int length, ...);
    bool getJSONfValue(float *output, int length, ...);
};

class DllExport AeCommonEncode {
    SINGLETON_CLASS(AeCommonEncode);

    AeXMLNode *decodeXML(const char *buffer, int &index, AeXMLNode *parent = nullptr);
    AeJSONNode *decodeJSON(const char *buffer, int &index);
    // QeAssetModel* decodeOBJ(char* buffer);
    // QeAssetModel* decodeGLB(char* buffer);
    // QeAssetMaterial* decodeMTL(char* buffer);
    std::vector<unsigned char> decodeJPEG(unsigned char *buffer, size_t size, int *width, int *height, int *bytes);
    std::vector<unsigned char> decodeBMP(unsigned char *buffer, int *width, int *height, int *bytes);
    std::vector<unsigned char> decodePNG(unsigned char *buffer, int *width, int *height, int *bytes);
    std::vector<unsigned char> decodeDeflate(unsigned char *in, unsigned int size);

    int readBits(const unsigned char *stream, size_t *bitPointer, size_t readCount, bool bLeft = false, bool bNegative = false);
    std::string trim(std::string s);

    template <class T>
    T ConvertTo(const std::string &str);

    template <class T>
    std::vector<T> split(std::string s, std::string delim);

    template <class T>
    std::string combine(std::vector<T> &ss, std::string delim);
};
#define COM_ENCODE AeCommonEncode::getInstance()

class DllExport QeTimer {
   public:
    QeTimer();
    ~QeTimer();
    int timerMilliSecond = 0;
    std::chrono::steady_clock::time_point *lastTime;

    std::chrono::steady_clock::time_point getNowTime();
    void initTime();
    void setTimer(int timerMilliSecond);
    int getPassTime();
    bool checkTimer(int &passMilliSecond);
};

class DllExport AeFile {
   public:
    AeFile();
    ~AeFile();

    std::ofstream *ofile;
    std::string *output_path;
    bool open(const char *output_path);
    bool isOpen();
    bool addNewLine(const char *s);
    void close();
};

class DllExport AeLogListener {
   public:
    virtual void updateLog(const char *msg) {}
};

class DllExport AeLog {
    SINGLETON_CLASS(AeLog);

    AeFile *file;
    std::vector<AeLogListener *> *listeners;

    void addListener(AeLogListener &listener);
    void removeListener(AeLogListener &listener);
    void setOutput(AeXMLNode &node, const char *output_name = "",
                   const char *turn_on_xml_setting_path = "setting.environment.outputLog",
                   const char *output_path_xml_setting_path = "setting.path.log");
    void switchOutput(bool turn_on, const char *output_path = nullptr);
    std::string stack(int from, int to);
    void print(std::string &msg, bool bShowStack = false, int stackLevel = 5);

    bool isOutput();
};
#define LOGOBJ AeLog::getInstance()
#define LOG(msg) LOGOBJ.print(std::string("") + msg)
#define STACK(msg) LOGOBJ.print(std::string("") + msg, true)
// std::terminate();

#ifndef NDEBUG
/*
static void setterminate() {
    std::cerr << "Unhandled exception\n";
    abort();  // forces abnormal termination
}
std::set_terminate(setterminate);
*/
#define ASSERT_PRINT(condition, message)                                                                   \
    std::ostringstream oss;                                                                                \
    oss << "Assertion `" #condition "` failed in " << __FILE__ << " line " << __LINE__ << ": " << message; \
    if (errno) {                                                                                           \
        oss << ": " << strerror(errno);                                                                    \
        errno = 0;                                                                                         \
    }                                                                                                      \
    STACK(oss.str());                                                                                      \
    std::terminate();

#define ASSERT(condition, message)       \
    if (!(condition)) {                  \
        ASSERT_PRINT(condition, message) \
    }
#define ASSERT_NULL(condition) ASSERT(condition, "NULL")
#else
#define ASSERT(condition, message)
#define ASSERT_NULL(condition)
#endif

namespace AeLib {
std::string DllExport toString(const int &i);
std::string DllExport operator+(std::string const &a, const int &b);
std::string DllExport operator+(std::string const &a, const size_t &b);
std::string DllExport operator+(std::string const &a, const float &b);
std::string DllExport operator+(std::string const &a, const double &b);
std::string DllExport operator+(std::string const &a, const char *b);
std::string DllExport operator+=(std::string const &a, const int &b);
std::string DllExport operator+=(std::string const &a, const size_t &b);
std::string DllExport operator+=(std::string const &a, const float &b);
std::string DllExport operator+=(std::string const &a, const double &b);
std::string DllExport operator+=(std::string const &a, const char *b);

template <class T>
int DllExport findElementFromVector(std::vector<T> &vec, T element);

template <class T>
bool DllExport eraseElementFromVector(std::vector<T> &vec, T element);
};  // namespace AeLib
using namespace AeLib;

#include "template_define.h"
