//
//  MyoManager.cpp
//  CinderMyoTest
//
//  Created by Mike Allison on 11/19/14.
//
//

#include "MyoManager.h"

using namespace myo;
using namespace ci;
using namespace ci::app;
using namespace std;

Manager::Manager( unsigned int read_rate,  DeviceListener* custom_listener )
:mReadRate(read_rate),
mRun(true)
{

    mHub = new myo::Hub("");
    
    if( custom_listener ){
        CI_LOG_V("We have a custom listener");
        mHub->addListener(custom_listener);
    }else{
        CI_LOG_V("Creating default listener");
        mListener = ArmbandListenerRef( new ArmbandListener() );
        mHub->addListener(mListener.get());
    }
    
    mListeningThread = std::async(std::launch::async, std::bind(&Manager::listen, this));
    
}

void Manager::listen(){
    
    while (mRun) {
        mHub->run( mReadRate );
        usleep(1000);
    }
    
}

Manager::~Manager(){
    mRun = false;
    mListeningThread.get();
    delete mHub;
}

Armband::Armband(int _id):mId(_id){
}

void ArmbandListener::onPair(Myo* myo, uint64_t timestamp, FirmwareVersion firmwareVersion) {
    CI_LOG_V("A Myo Armband has paired, timestamp: "+to_string(timestamp)+" with firmware: "+to_string(firmwareVersion.firmwareVersionMajor) +"."+to_string(firmwareVersion.firmwareVersionMinor) +"."+to_string(firmwareVersion.firmwareVersionPatch));
    mArmbands.insert(std::make_pair(myo, ArmbandRef(new Armband( mArmbands.size() ))));
    if( !mOnPair.empty() )
        mOnPair();
}

void ArmbandListener::onUnpair(Myo* myo, uint64_t timestamp){
    
    CI_LOG_V("A Myo Armband has unpaired, timestamp: "+to_string(timestamp));
    mArmbands.erase(myo);
    if( !mOnUnpair.empty() )
        mOnUnpair();
    
}

void ArmbandListener::onConnect(Myo* myo, uint64_t timestamp, FirmwareVersion firmwareVersion){
    CI_LOG_V("A Myo Armband has connected, timestamp: "+to_string(timestamp));
    if( mArmbands.find(myo) != mArmbands.end() )
        mArmbands.find(myo)->second->onConnect(timestamp, firmwareVersion);
    else{
        mArmbands.insert(std::make_pair(myo, ArmbandRef(new Armband( mArmbands.size() ))));
        mArmbands.find(myo)->second->onConnect(timestamp, firmwareVersion);
    }
}

void ArmbandListener::onDisconnect(Myo* myo, uint64_t timestamp){
    CI_LOG_V("A Myo Armband has disconnected, timestamp: "+to_string(timestamp));
    mArmbands.find(myo)->second->onDisconnect(timestamp);
}

void ArmbandListener::onArmSync(Myo* myo, uint64_t timestamp, Arm arm, XDirection xDirection){
    
    std::string armname = "";
    switch (arm) {
        case Arm::armLeft:
            armname = "left";
            break;
        case Arm::armRight:
            armname = "right";
            break;
        case Arm::armUnknown:
            armname = "unknown";
            break;
    }
    
    
    std::string axis = "";
    switch (xDirection) {
        case xDirectionTowardElbow:
            axis = "toward elbow";
            break;
        case xDirectionTowardWrist:
            axis = "toward wrist";
            break;
        case xDirectionUnknown:
            axis = "unknown";
            break;
    }
    
    CI_LOG_V("A Myo Armband has synced, arm: "+armname+" and x axis is facing: "+axis );
    mArmbands.find(myo)->second->onArmSync(timestamp, arm, xDirection);
}

void ArmbandListener::onArmUnsync(Myo* myo, uint64_t timestamp){
    CI_LOG_V("A Myo Armband has unsynced from arm arm");
    mArmbands.find(myo)->second->onArmUnsync(timestamp);
}

void ArmbandListener::onPose(Myo* myo, uint64_t timestamp, Pose pose){
    mArmbands.find(myo)->second->onPose(timestamp, pose);
}

void ArmbandListener::onOrientationData(Myo* myo, uint64_t timestamp, const Quaternion<float>& rotation){
    mArmbands.find(myo)->second->onOrientationData(timestamp, rotation);
}

void ArmbandListener::onAccelerometerData(Myo* myo, uint64_t timestamp, const Vector3<float>& accel){
    mArmbands.find(myo)->second->onAccelerometerData(timestamp, accel);
}

void ArmbandListener::onGyroscopeData(Myo* myo, uint64_t timestamp, const Vector3<float>& gyro){
    mArmbands.find(myo)->second->onGyroscopeData(timestamp, gyro);
}

void ArmbandListener::onRssi(Myo* myo, uint64_t timestamp, int8_t rssi) {
    mArmbands.find(myo)->second->onRssi(timestamp,  rssi);
}
