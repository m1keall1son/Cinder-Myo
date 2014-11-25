#include "cinder/app/AppNative.h"
#include "cinder/app/RendererGl.h"
#include "cinder/gl/gl.h"

#include "cinder/gl/Batch.h"
#include "cinder/gl/Shader.h"

#include "MyoManager.h"
#include "cinder/TriMesh.h"
#include "cinder/ObjLoader.h"
#include "cinder/Perlin.h"


using namespace ci;
using namespace ci::app;
using namespace std;

//hold some data for an amrband
struct MyoArmband {
    quat mRotation;
    vec3 mAcceleration;
    vec3 mGyroScope;
    myo::Pose mCurrentPose;
    myo::Pose mPrevPose;
    myo::XDirection mDirection;
    myo::Arm mArm;
};

class MyoBlockTestApp : public AppNative {
public:
	void setup() override;
	void mouseDown( MouseEvent event ) override;
	void update() override;
	void draw() override;
    
    //create a function to pass as a callback for a new myo armband connection
    void myoConnected();
    
    myo::ManagerRef mMyoManager;
    
    MyoArmband mArmband;
    
    gl::BatchRef mHelloWorld;
    
    CameraPersp mCam;
    
    bool seekHome = false;
    
    Perlin perlin;
    
};

void MyoBlockTestApp::myoConnected(){
    
    //connect callbacks for the armbands
    //getArmband defaults to armband 0;
    auto armband = mMyoManager->getArmband();
    
    if(armband){
        
        console() << "Myo id: "+ to_string( armband->getID() ) << endl;
        armband->connectOrientationHandler( [&]( const quat& rot ){ mArmband.mRotation = rot; mArmband.mRotation *= quat(rotate( 25.f, vec3(0,0,1) )); } );
        armband->connectArmSyncHandler( [&](const myo::Arm& arm, const myo::XDirection& dir ){ mArmband.mArm = arm; mArmband.mDirection = dir; } );
        armband->connectPoseHandler( [&](const myo::Pose& pose){
            
            if(pose != mArmband.mPrevPose){
                
                mArmband.mPrevPose = mArmband.mCurrentPose;
                mArmband.mCurrentPose = pose;
                
                switch (mArmband.mCurrentPose.type()) {
                    case myo::Pose::Type::fingersSpread:
                    {
                    }
                        break;
                    case myo::Pose::Type::fist:
                    {
                    }
                    default:
                        break;
                }
            }
            
        });
        armband->connectAccelerationHandler( [&](const vec3& accel ){ mArmband.mAcceleration = accel; } );
        
    }else{
        CI_LOG_E("There was a problem accessing the armband");
    }
    
    mCam.setPerspective(60, getWindowAspectRatio(), .1, 10000);
    mCam.lookAt(vec3(0,0,50), vec3(0,0,0));
    
}

void MyoBlockTestApp::setup()
{
    getWindow()->setFullScreen();
    
    //create a batch to draw
    mHelloWorld = gl::Batch::create( ObjLoader( loadAsset("helloWorld.obj") ), gl::GlslProg::create(loadAsset("render.vert"), loadAsset("render.frag")) );
    
    //create a manager and default device listener
    mMyoManager = myo::Manager::create();
    
    //this will fire once each myo connection
    mMyoManager->connectOnPairHandler(std::bind( &MyoBlockTestApp::myoConnected, this ));
    
    
    gl::enableDepthRead();
    gl::enableDepthWrite();
    
}

void MyoBlockTestApp::mouseDown( MouseEvent event )
{
}

void MyoBlockTestApp::update()
{
    
    console() << "---------------------" << endl;
    console() << mArmband.mRotation << endl;
    console() << mArmband.mCurrentPose.toString() << endl;
    console() << mArmband.mAcceleration << endl;
    
}

void MyoBlockTestApp::draw()
{
	gl::clear( Color( 0, 0, 0 ) );
    
    gl::ScopedMatrices push;
    
    gl::setMatrices(mCam);
    
    {
        gl::ScopedModelMatrix model;
        gl::multModelMatrix(toMat4(mArmband.mRotation));
        mHelloWorld->draw();
    }
    
}

CINDER_APP_NATIVE( MyoBlockTestApp, RendererGl )
