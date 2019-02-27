// Contains utilities for measuring the frame rate.
#pragma once

#include <genapi/Synch.h>
#include <winsock2.h>
#include <windows.h>
#include <vector>

//class CStopWatch
//{
//public:
//    /// Constructor, starts time measurement
//    CStopWatch(){ Start(); }
//
//    /// Start the stop watch 
//    void Start(){ QueryPerformanceCounter(&m_StartTime); }
//
//    /// Stop. The elapsed time is returned. The stop watch may be started again
//    double Stop(bool StartAgain)
//    {
//        QueryPerformanceCounter(&m_StopTime);
//        double theElapsedTime = ElapsedTime();
//        if(StartAgain)
//            m_StartTime = m_StopTime; 
//        return theElapsedTime;
//    }
//
//    /// Return the elapsed time in seconds between start() and stop()
//    double ElapsedTime()
//    {
//        LARGE_INTEGER timerFrequency;
//        QueryPerformanceFrequency(&timerFrequency);
//
//        __int64 oldTicks = ((__int64)m_StartTime.HighPart << 32) + (__int64)m_StartTime.LowPart;
//        __int64 newTicks = ((__int64)m_StopTime.HighPart << 32) + (__int64)m_StopTime.LowPart;
//        long double timeDifference = (long double) (newTicks - oldTicks);
//
//        long double ticksPerSecond = (long double) (((__int64)timerFrequency.HighPart << 32) 
//            + (__int64)timerFrequency.LowPart);
//
//        return (double)(timeDifference / ticksPerSecond);
//    }
//
//private:
//    /// zero-point for time measurment
//    LARGE_INTEGER m_StartTime;
//
//    /// last time stamp
//    LARGE_INTEGER m_StopTime;
//};
//
//// Template class to calculate a moving average
///*! \param T Type of the measurement data (typically double or float)
//\param span Number of samples used of computing the mean
//The class is thread safe.
//*/
//template <class T, unsigned int span = 10>
//class CMovingAvg
//{
//
//public:
//    // Default constructor
//    CMovingAvg() : m_v(span) 
//    {
//        Reset();
//    }
//
//    // Add a data item to the moving average
//    void Add(T sample) 
//    { 
//        GenApi::AutoLock lock(m_lock);
//        int idx = m_n % span;
//        if ( m_n < span )
//        {
//            m_v[idx] = sample;
//            m_Sum += sample;
//        }
//        else
//        {
//            m_Sum -= m_v[idx];
//            m_Sum += sample;
//            m_v[idx] = sample;
//        }
//        m_n++;
//    }
//
//    // Get the average
//    double Avg() {
//        double res = 0;
//        GenApi::AutoLock lock(m_lock);
//        if ( m_n != 0 )
//        {
//            res = m_n < span ? m_Sum / (double) m_n : m_Sum / (double) span;
//        }
//        return res;
//    }
//
//    // Reset the moving average clearing the history
//    void Reset() {
//        GenApi::AutoLock lock(m_lock);
//        m_Sum = 0.0;
//        m_n = 0;
//    }
//
//protected:
//    // The current sum
//    double m_Sum;
//
//    // Number of measurement data
//    unsigned int m_n;
//
//    // Vector holding #span data items
//    std::vector<T> m_v;
//
//    // Critical section guarding the access to the class
//    GenApi::CLock m_lock;  
//};
//
