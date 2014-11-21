//
//  MyoManager.h
//  CinderMyoTest
//
//  Created by Mike Allison on 11/19/14.
//
//

#pragma once
#include <myo/myo.hpp>
#include "cinder/Log.h"

namespace myo {
    
    using ArmbandRef = std::shared_ptr< class Armband >;
    using ManagerRef = std::shared_ptr<class Manager>;

    using ArmbandEventSignal = cinder::signals::signal< void () >;
    using ArmbandEventHandler = std::function<void()>;
    
    using ArmbandOnArmSyncEventSignal = cinder::signals::signal< void ( const Arm&, const XDirection& ) >;
    using ArmbandOnArmSyncEventHandler = std::function<void(const Arm&, const XDirection&)>;
    
    using ArmbandOnPoseEventSignal = cinder::signals::signal< void ( const Pose& ) >;
    using ArmbandOnPoseEventHandler = std::function<void(const Pose&)>;
    
    using ArmbandOnOrientationEventSignal = cinder::signals::signal< void ( const ci::quat& ) >;
    using ArmbandOnOrientationEventHandler = std::function<void(const ci::quat&)>;

    using ArmbandOnAccelerometerEventSignal = cinder::signals::signal< void ( const ci::vec3& ) >;
    using ArmbandOnAccelerometerEventHandler = std::function<void(const ci::vec3&)>;
    
    using ArmbandOnGyroEventSignal = cinder::signals::signal< void ( const ci::vec3& ) >;
    using ArmbandOnGyroEventHandler = std::function<void(const ci::vec3&)>;
    
    using ArmbandOnRSSIEventSignal = cinder::signals::signal< void ( const uint8_t& ) >;
    using ArmbandOnRSSIEventHandler = std::function<void(const uint8_t&)>;
    
    using ArmbandListenerRef = std::shared_ptr<class ArmbandListener >;
    
    class ArmbandListener : public myo::DeviceListener {
    private:
        friend Manager;
        
        void connectOnPair( ArmbandEventHandler handler ){ mOnPair.connect( handler );  }
        
        /// Called when a Myo has been paired.
        void onPair(Myo* myo, uint64_t timestamp, FirmwareVersion firmwareVersion) override;
    
        /// Called when a Myo has been unpaired.
        void onUnpair(Myo* myo, uint64_t timestamp) override;
    
        /// Called when a paired Myo has been connected.
        void onConnect(Myo* myo, uint64_t timestamp, FirmwareVersion firmwareVersion)override;
    
        /// Called when a paired Myo has been disconnected.
        void onDisconnect(Myo* myo, uint64_t timestamp)override;
        
        /// Called when a paired Myo recognizes that it is on an arm.
        void onArmSync(Myo* myo, uint64_t timestamp, Arm arm, XDirection xDirection)override;
    
        /// Called when a paired Myo is moved or removed from the arm.
        void onArmUnsync(Myo* myo, uint64_t timestamp)override;
    
        /// Called when a paired Myo has provided a new pose.
        void onPose(Myo* myo, uint64_t timestamp, Pose pose) override;
    
        /// Called when a paired Myo has provided new orientation data.
        void onOrientationData(Myo* myo, uint64_t timestamp, const Quaternion<float>& rotation)override;
    
        /// Called when a paired Myo has provided new accelerometer data in units of g.
        void onAccelerometerData(Myo* myo, uint64_t timestamp, const Vector3<float>& accel)override;
    
        /// Called when a paired Myo has provided new gyroscope data in units of deg/s.
        void onGyroscopeData(Myo* myo, uint64_t timestamp, const Vector3<float>& gyro) override;
    
        /// Called when a paired Myo has provided a new RSSI value.
        /// @see Myo::requestRssi() to request an RSSI value from the Myo.
        void onRssi(Myo* myo, uint64_t timestamp, int8_t rssi) override;
    
        std::map< Myo*, ArmbandRef> mArmbands;
        ArmbandEventSignal mOnPair;
        ArmbandEventSignal mOnUnpair;

    };



    class Armband : public std::enable_shared_from_this<Armband> {
    public:
        
        inline void connectPairHandler( ArmbandEventHandler handler ){ mPairSignal.connect(handler); }
        inline void connectUnpairHandler( ArmbandEventHandler handler ){ mUnpairSignal.connect(handler); }
        inline void connectConnectHandler( ArmbandEventHandler handler ){ mConnectSignal.connect(handler); }
        inline void connectDisconnectHandler( ArmbandEventHandler handler ){ mDisconnectSignal.connect(handler); }
        inline void connectArmSyncHandler( ArmbandOnArmSyncEventHandler handler ){ mArmSyncSignal.connect(handler); }
        inline void connectArmUnsyncHandler( ArmbandEventHandler handler ){ mArmUnsyncSignal.connect(handler); }
        inline void connectPoseHandler( ArmbandOnPoseEventHandler handler ){ mPoseSignal.connect(handler); }
        inline void connectOrientationHandler( ArmbandOnOrientationEventHandler handler ){ mOrientationSignal.connect(handler); }
        inline void connectAccelerationHandler( ArmbandOnAccelerometerEventHandler handler ){ mAccelerationSignal.connect(handler); }
        inline void connectGyroHandler( ArmbandOnGyroEventHandler handler ){ mGyroSignal.connect(handler); }
        inline void connectRSSIHandler( ArmbandOnRSSIEventHandler handler ){ mRSSISignal.connect(handler); }
        
        const int& getID(){ return mId; }
        
    private:
        
        inline void onConnect(uint64_t timestamp, FirmwareVersion firmwareVersion) {
            if(!mConnectSignal.empty())
                mConnectSignal();
        }

        inline void onDisconnect(uint64_t timestamp) {
            if(!mDisconnectSignal.empty())
                mDisconnectSignal();
        }

        inline void onArmSync(uint64_t timestamp, Arm arm, XDirection xDirection) {
            if(!mArmSyncSignal.empty())
                mArmSyncSignal(arm, xDirection);
        }
        
        inline void onArmUnsync(uint64_t timestamp) {
            if(!mArmUnsyncSignal.empty())
                mArmUnsyncSignal();
        }
        
        inline void onPose(uint64_t timestamp, Pose pose) {
            if(!mPoseSignal.empty())
                mPoseSignal( pose );
        }
        
        inline void onOrientationData(uint64_t timestamp, const Quaternion<float>& rotation) {
            if(!mOrientationSignal.empty())
                mOrientationSignal( ci::quat( rotation.w(), rotation.x(), rotation.y(), rotation.z() ) );
        }
        
        inline void onAccelerometerData(uint64_t timestamp, const Vector3<float>& accel) {
            if(!mAccelerationSignal.empty())
                mAccelerationSignal( ci::vec3( accel.x(), accel.y(), accel.z() ) );
        }

        inline void onGyroscopeData( uint64_t timestamp, const Vector3<float>& gyro) {
            if(!mGyroSignal.empty())
                mGyroSignal( ci::vec3( gyro.x(), gyro.y(), gyro.z() )  );
        }
        
        inline void onRssi(uint64_t timestamp, int8_t rssi){
            if(!mRSSISignal.empty())
                mRSSISignal( rssi );
        }

        Armband(int _id);
        
        ArmbandEventSignal mPairSignal;
        ArmbandEventSignal mUnpairSignal;
        ArmbandEventSignal mConnectSignal;
        ArmbandEventSignal mDisconnectSignal;
        ArmbandOnArmSyncEventSignal mArmSyncSignal;
        ArmbandEventSignal mArmUnsyncSignal;
        ArmbandOnPoseEventSignal mPoseSignal;
        ArmbandOnOrientationEventSignal mOrientationSignal;
        ArmbandOnAccelerometerEventSignal mAccelerationSignal;
        ArmbandOnGyroEventSignal mGyroSignal;
        ArmbandOnRSSIEventSignal mRSSISignal;
        
        int mId;
        
        friend class ArmbandListener;
        
    };
    
    
    class Manager{
        
    public:
        
        //! create a manager passing reading rate in millis, default is 60 reads per second, if no custom listener is present a default will be created
        static ManagerRef create( unsigned int read_rate = 1000/60, DeviceListener* custom_listener = nullptr ){ return ManagerRef( new Manager(read_rate, custom_listener) ); }
        
        inline uint32_t getNumArmbands(){ return mListener->mArmbands.size(); }
        
        inline void connectOnPairHandler( ArmbandEventHandler handler ){ mListener->mOnPair.connect(handler); }
        inline void connectOnUnpairHandler( ArmbandEventHandler handler ){ mListener->mOnUnpair.connect(handler); }

        inline ArmbandRef getArmband( int index = 0 ){
            if(mListener){
                if( index < mListener->mArmbands.size() ){
                    auto it = mListener->mArmbands.begin();
                    std::advance(it, index);
                    return it->second;
                }else{
                    return mListener->mArmbands.end()->second;
                }
            }else{
                CI_LOG_E("Using Custom Listener, Manager has no Armbands");
                return nullptr;
            }
        }
        
        ~Manager();
        
    private:
        
        Manager( unsigned int read_rate,  DeviceListener* custom_listener );
        
        std::future< void > mHubThread;
        std::future< void > mListeningThread;
        
        void listen();
        
        ArmbandListenerRef mListener;

        myo::Hub* mHub;
        
        bool mRun;
        
        unsigned int mReadRate;
        
        friend ArmbandListener;
        
    };
    
}