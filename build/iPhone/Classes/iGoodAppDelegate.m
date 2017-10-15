//
//  iGoodAppDelegate.m
//  iGood
//
//  Created by waync on 2009/7/16.
//  Copyright __MyCompanyName__ 2009. All rights reserved.
//

#import "iGoodAppDelegate.h"
#import "EAGLView.h"

@implementation iGoodAppDelegate

@synthesize window;
@synthesize glView;

- (void)applicationDidFinishLaunching:(UIApplication *)application {
    
	glView.animationInterval = 1.0 / 60.0;
	[glView startAnimation];
}


- (void)applicationWillResignActive:(UIApplication *)application {
	glView.animationInterval = 1.0 / 5.0;
}


- (void)applicationDidBecomeActive:(UIApplication *)application {
	glView.animationInterval = 1.0 / 60.0;
}


- (void)dealloc {
	[window release];
	[glView release];
	[super dealloc];
}

@end
