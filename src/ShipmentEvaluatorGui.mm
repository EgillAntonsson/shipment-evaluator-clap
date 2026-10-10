#import <AppKit/AppKit.h>
#include "ShipmentEvaluatorGui.h"
#include <algorithm>
#include <cmath>

namespace {
constexpr CGFloat kWindowWidth = 640.0;
constexpr CGFloat kWindowHeight = 500.0;
constexpr CGFloat kPi = 3.14159265358979323846;

void draw_text(NSString *text,
               NSRect rect,
               NSFont *font,
               NSColor *color,
               NSTextAlignment alignment) {
    NSMutableParagraphStyle *paragraph = [[NSMutableParagraphStyle alloc] init];
    paragraph.alignment = alignment;
    [text drawInRect:rect withAttributes:@{
        NSFontAttributeName: font,
        NSForegroundColorAttributeName: color,
        NSParagraphStyleAttributeName: paragraph
    }];
}
}

@interface ShipmentMeterView : NSView {
@private
    const std::atomic<double> *meter_;
    NSTimer *refresh_timer_;
}
- (instancetype)initWithMeter:(const std::atomic<double> *)meter;
- (void)refreshMeter:(NSTimer *)timer;
- (void)updateRefreshTimer;
@end

@implementation ShipmentMeterView

- (instancetype)initWithMeter:(const std::atomic<double> *)meter {
    self = [super initWithFrame:NSMakeRect(0.0, 0.0, kWindowWidth, kWindowHeight)];
    if (self) {
        meter_ = meter;
        self.wantsLayer = YES;
        self.layer.backgroundColor = [NSColor colorWithCalibratedRed:0.035
                                                               green:0.045
                                                                blue:0.045
                                                               alpha:1.0].CGColor;
    }
    return self;
}

- (BOOL)isOpaque {
    return YES;
}

- (void)viewDidMoveToWindow {
    [super viewDidMoveToWindow];
    [self updateRefreshTimer];
}

- (void)setHidden:(BOOL)hidden {
    [super setHidden:hidden];
    [self updateRefreshTimer];
}

- (void)updateRefreshTimer {
    if (self.window && !self.hidden && !refresh_timer_) {
        refresh_timer_ = [NSTimer scheduledTimerWithTimeInterval:1.0 / 30.0
                                                         target:self
                                                       selector:@selector(refreshMeter:)
                                                       userInfo:nil
                                                        repeats:YES];
    } else if (refresh_timer_) {
        [refresh_timer_ invalidate];
        refresh_timer_ = nil;
    }
}

- (void)refreshMeter:(NSTimer *)timer {
    (void)timer;
    [self setNeedsDisplay:YES];
}

- (void)drawRect:(NSRect)dirtyRect {
    (void)dirtyRect;
    const NSRect bounds = self.bounds;
    NSColor *const background = [NSColor colorWithCalibratedRed:0.035
                                                          green:0.045
                                                           blue:0.045
                                                          alpha:1.0];
    NSColor *const panel = [NSColor colorWithCalibratedRed:0.075
                                                     green:0.09
                                                      blue:0.085
                                                     alpha:1.0];
    NSColor *const muted = [NSColor colorWithCalibratedRed:0.62
                                                     green:0.67
                                                      blue:0.61
                                                     alpha:1.0];
    NSColor *const accent = [NSColor colorWithCalibratedRed:0.73
                                                      green:0.83
                                                       blue:0.29
                                                      alpha:1.0];

    [background setFill];
    NSRectFill(bounds);
    [panel setFill];
    [[NSBezierPath bezierPathWithRoundedRect:NSInsetRect(bounds, 18.0, 18.0)
                                     xRadius:18.0
                                     yRadius:18.0] fill];

    draw_text(@"THE SHIPMENT EVALUATOR",
              NSMakeRect(30.0, 446.0, bounds.size.width - 60.0, 26.0),
              [NSFont boldSystemFontOfSize:19.0],
              [NSColor colorWithCalibratedWhite:0.94 alpha:1.0],
              NSTextAlignmentCenter);
    draw_text(@"A highly scientific, completely predictable assessment",
              NSMakeRect(30.0, 421.0, bounds.size.width - 60.0, 20.0),
              [NSFont systemFontOfSize:12.0],
              muted,
              NSTextAlignmentCenter);

    const double meter = std::clamp(meter_->load(std::memory_order_relaxed), 0.0, 11.0);
    const NSPoint center = NSMakePoint(bounds.size.width * 0.5, 273.0);
    const CGFloat arcRadius = 124.0;
    NSBezierPath *arc = [NSBezierPath bezierPath];
    [arc appendBezierPathWithArcWithCenter:center
                                    radius:arcRadius
                                startAngle:180.0
                                  endAngle:0.0
                                 clockwise:NO];
    arc.lineWidth = 5.0;
    [muted setStroke];
    [arc stroke];

    for (int tick = 0; tick <= 11; ++tick) {
        const double angle = kPi * (1.0 - static_cast<double>(tick) / 11.0);
        const CGFloat outerRadius = arcRadius + 2.0;
        const CGFloat innerRadius = arcRadius - (tick % 2 == 0 ? 14.0 : 8.0);
        const NSPoint start = NSMakePoint(center.x + std::cos(angle) * outerRadius,
                                          center.y + std::sin(angle) * outerRadius);
        const NSPoint end = NSMakePoint(center.x + std::cos(angle) * innerRadius,
                                        center.y + std::sin(angle) * innerRadius);
        NSBezierPath *mark = [NSBezierPath bezierPath];
        [mark moveToPoint:start];
        [mark lineToPoint:end];
        mark.lineWidth = tick % 2 == 0 ? 2.0 : 1.0;
        NSColor *tickColor = static_cast<double>(tick) <= meter ? accent : muted;
        [tickColor setStroke];
        [mark stroke];

        const CGFloat labelRadius = arcRadius + 28.0;
        const NSPoint labelPoint = NSMakePoint(center.x + std::cos(angle) * labelRadius - 12.0,
                                               center.y + std::sin(angle) * labelRadius - 8.0);
        draw_text([NSString stringWithFormat:@"%d", tick],
                  NSMakeRect(labelPoint.x, labelPoint.y, 24.0, 16.0),
                  [NSFont monospacedDigitSystemFontOfSize:11.0 weight:NSFontWeightMedium],
                  muted,
                  NSTextAlignmentCenter);
    }

    const double needleAngle = kPi * (1.0 - meter / 11.0);
    const NSPoint needleTip = NSMakePoint(center.x + std::cos(needleAngle) * 105.0,
                                          center.y + std::sin(needleAngle) * 105.0);
    NSBezierPath *needle = [NSBezierPath bezierPath];
    [needle moveToPoint:center];
    [needle lineToPoint:needleTip];
    needle.lineWidth = 4.0;
    needle.lineCapStyle = NSLineCapStyleRound;
    [accent setStroke];
    [needle stroke];

    NSBezierPath *hub = [NSBezierPath bezierPathWithOvalInRect:
        NSMakeRect(center.x - 9.0, center.y - 9.0, 18.0, 18.0)];
    [accent setFill];
    [hub fill];

    draw_text(@"THE EVALUATION METER",
              NSMakeRect(40.0, 388.0, bounds.size.width - 80.0, 20.0),
              [NSFont boldSystemFontOfSize:12.0],
              accent,
              NSTextAlignmentCenter);
    draw_text([NSString stringWithFormat:@"%.1f", meter],
              NSMakeRect(center.x - 44.0, center.y - 55.0, 88.0, 32.0),
              [NSFont monospacedDigitSystemFontOfSize:23.0 weight:NSFontWeightSemibold],
              [NSColor colorWithCalibratedWhite:0.94 alpha:1.0],
              NSTextAlignmentCenter);

    draw_text(@"EVALUATION RESULT",
              NSMakeRect(35.0, 101.0, bounds.size.width - 70.0, 19.0),
              [NSFont boldSystemFontOfSize:11.0],
              muted,
              NSTextAlignmentCenter);
    if (meter >= 11.0) {
        draw_text(@"♬ ♫ ♪ ♩  →  SHIP IT !  ←  ♩ ♪ ♫ ♬",
                  NSMakeRect(28.0, 64.0, bounds.size.width - 56.0, 28.0),
                  [NSFont boldSystemFontOfSize:18.0],
                  accent,
                  NSTextAlignmentCenter);
    } else {
        draw_text(@"Start playback to begin the evaluation",
                  NSMakeRect(28.0, 65.0, bounds.size.width - 56.0, 24.0),
                  [NSFont systemFontOfSize:14.0],
                  muted,
                  NSTextAlignmentCenter);
    }
    draw_text(@"A fixed result. No audio is processed.",
              NSMakeRect(30.0, 32.0, bounds.size.width - 60.0, 17.0),
              [NSFont systemFontOfSize:10.0],
              muted,
              NSTextAlignmentCenter);
}
@end

ShipmentEvaluatorGui::ShipmentEvaluatorGui(const std::atomic<double> *meter)
    : meter_(meter) {}

ShipmentEvaluatorGui::~ShipmentEvaluatorGui() {
    destroy();
}

bool ShipmentEvaluatorGui::create() {
    if (view_)
        return true;

    ShipmentMeterView *view = [[ShipmentMeterView alloc] initWithMeter:meter_];
    if (!view)
        return false;
    view_ = (__bridge_retained void *)view;
    return true;
}

void ShipmentEvaluatorGui::destroy() {
    if (!view_)
        return;

    ShipmentMeterView *view = (__bridge_transfer ShipmentMeterView *)view_;
    view_ = nullptr;
    [view removeFromSuperview];
}

bool ShipmentEvaluatorGui::get_size(uint32_t *width, uint32_t *height) const {
    if (!view_ || !width || !height)
        return false;

    *width = static_cast<uint32_t>(kWindowWidth);
    *height = static_cast<uint32_t>(kWindowHeight);
    return true;
}

bool ShipmentEvaluatorGui::set_size(uint32_t width, uint32_t height) {
    if (!view_ || width != static_cast<uint32_t>(kWindowWidth) ||
        height != static_cast<uint32_t>(kWindowHeight))
        return false;

    ShipmentMeterView *view = (__bridge ShipmentMeterView *)view_;
    [view setFrameSize:NSMakeSize(width, height)];
    return true;
}

bool ShipmentEvaluatorGui::set_parent(void *parent_view) {
    if (!view_ || !parent_view)
        return false;

    ShipmentMeterView *view = (__bridge ShipmentMeterView *)view_;
    NSView *parent = (__bridge NSView *)parent_view;
    [view removeFromSuperview];
    view.frame = parent.bounds;
    view.autoresizingMask = NSViewWidthSizable | NSViewHeightSizable;
    [parent addSubview:view];
    return true;
}

bool ShipmentEvaluatorGui::show() {
    if (!view_)
        return false;

    ShipmentMeterView *view = (__bridge ShipmentMeterView *)view_;
    view.hidden = NO;
    return true;
}

bool ShipmentEvaluatorGui::hide() {
    if (!view_)
        return false;

    ShipmentMeterView *view = (__bridge ShipmentMeterView *)view_;
    view.hidden = YES;
    return true;
}
