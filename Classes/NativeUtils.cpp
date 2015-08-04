/*
 * NativeUtils.cpp
 *
 *  Created on: Nov 22, 2014
 *      Author: reddevil_138
 */

#include "NativeUtils.h"

#if(CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID)
#include <jni.h>
#include <android/log.h>
#define CLASS_NAME "org/cocos2dx/cpp/AppActivity"
#endif

using namespace cocos2d;

#pragma mark - Sign in and Sign out.
bool NativeUtils::isSignedIn() {
#if(CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID)
	return JniHelpers::jniCommonBoolCall(
			"isSignedIn",
			CLASS_NAME);
#endif
	return false;
}

void NativeUtils::signIn() {
#if(CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID)
	JniHelpers::jniCommonVoidCall(
			"gameServicesSignIn",
			CLASS_NAME);
#endif
}

void NativeUtils::signOut() {
#if(CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID)
	JniHelpers::jniCommonVoidCall(
			"gameServicesSignOut",
			CLASS_NAME);
#endif
}

#pragma mark - Submit score and achievements.
void NativeUtils::submitScore(const char* leaderboardID, long score) {
#if(CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID)
	JniHelpers::jniCommonVoidCall(
			"submitScore",
			CLASS_NAME,
			leaderboardID,
			score);
#endif
}

void NativeUtils::showLeaderboards() {
#if(CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID)
	JniHelpers::jniCommonVoidCall(
			"showLeaderboards",
			CLASS_NAME);
#endif
}

void NativeUtils::showLeaderboard(const char* leaderboardID) {
#if(CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID)
	JniHelpers::jniCommonVoidCall(
			"showLeaderboard",
			CLASS_NAME,
			leaderboardID);
#endif
}

