#import "JMView.h"
#include "jmlib.h"
#import "JMController.h"

@interface JMView(Private)

- (void)drawSiteSwap;
- (void)drawBall:(int)ball;

@end

@implementation JMView

- (id)initWithFrame:(NSRect)frameRect
{
	if ((self = [super initWithFrame:frameRect]) != nil)
	{
		[dataSource setFrame:frameRect];
	}
		
	return self;
}

- (void)setFrame:(NSRect)frameRect
{
	[super setFrame:frameRect];
	[dataSource setFrame:frameRect];
}

- (BOOL)isFlipped
{
	return YES;
}

- (void)drawRect:(NSRect)rect
{
	int ballNum;
	arm a= [dataSource bodyPartPositions];
	NSBezierPath *headPath = [NSBezierPath bezierPathWithOvalInRect:NSMakeRect(a.hx - a.hr, a.hy - a.hr, a.hr * 2, a.hr * 2)];
	NSBezierPath *rightArmPath = [NSBezierPath bezierPath];
	NSBezierPath *leftArmPath = [NSBezierPath bezierPath];
	
	[headPath setLineWidth:2.0];
	[rightArmPath setLineWidth:2.0];
	[leftArmPath setLineWidth:2.0];
	
	[rightArmPath setLineJoinStyle:NSRoundLineJoinStyle];
	[rightArmPath moveToPoint:NSMakePoint(a.rx[0], a.ry[0])];
	[leftArmPath setLineJoinStyle:NSRoundLineJoinStyle];
	[leftArmPath moveToPoint:NSMakePoint(a.lx[0], a.ly[0])];
	
	for (int i=1;i<=5;i++)
	{
		[rightArmPath lineToPoint:NSMakePoint(a.rx[i], a.ry[i])];
		[leftArmPath lineToPoint:NSMakePoint(a.lx[i], a.ly[i])];
	}
	
	[[NSColor blackColor] set];
	[NSBezierPath fillRect:rect];
	
	[self drawSiteSwap];

	[[NSColor whiteColor] set];
	[headPath stroke];
	[rightArmPath stroke];
	[leftArmPath stroke];
	
	for(ballNum = 0; ballNum < [dataSource numberOfBalls]; ballNum++)
	{
		[self drawBall:ballNum];
	}
}

- (void)drawSiteSwap
{
	if ([dataSource shouldShowPattern])
	{
		NSRect frame = [self frame];
		
		frame.size.width -= 10;
		frame.size.height -= 10;
		frame.origin.x += 5;
		frame.origin.y += 5;
		
		[[dataSource currentThrowString] drawInRect:frame];
		[[dataSource juggleStyleString] drawInRect:frame];
		[[dataSource numberOfBallsString] drawInRect:frame];
	}
}

- (void)drawBall:(int)ball
{
	NSPoint p = [dataSource ballPosition:ball];
	float radius = (float)[dataSource ballDiameter] / 2.0;
	
	NSBezierPath *ballPath = [NSBezierPath bezierPathWithOvalInRect:NSMakeRect(p.x, p.y, radius * 2, radius * 2)];
	
	[[dataSource ballColour:ball] set];
	[ballPath fill];
}

@end
