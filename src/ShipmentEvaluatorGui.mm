#import <AppKit/AppKit.h>
#include "ShipmentEvaluatorGui.h"
#include "EvaluationMeter.h"
#include <algorithm>
#include <cmath>

namespace {
constexpr CGFloat kWindowWidth = 640.0;
constexpr CGFloat kWindowHeight = 500.0;

struct RGBAColor {
    CGFloat red;
    CGFloat green;
    CGFloat blue;
    CGFloat alpha;
};

constexpr RGBAColor kBackgroundColor{0.035, 0.045, 0.045, 1.0};
constexpr RGBAColor kPanelColor{0.075, 0.09, 0.085, 1.0};
constexpr RGBAColor kMutedColor{0.62, 0.67, 0.61, 1.0};
constexpr RGBAColor kAccentColor{0.73, 0.83, 0.29, 1.0};
constexpr RGBAColor kTextColor{0.94, 0.94, 0.94, 1.0};
constexpr RGBAColor kKnobTopColor{0.42, 0.44, 0.41, 1.0};
constexpr RGBAColor kKnobBottomColor{0.13, 0.15, 0.14, 1.0};
constexpr RGBAColor kKnobIndicatorColor{0.95, 0.95, 0.87, 1.0};

NSColor *make_color(const RGBAColor &color) {
    return [NSColor colorWithCalibratedRed:color.red
                                    green:color.green
                                     blue:color.blue
                                    alpha:color.alpha];
}

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

void draw_amplifier_knob(NSPoint center, double angle) {
    constexpr CGFloat kFaceRadius = 49.0;
    NSRect face_rect = NSMakeRect(center.x - kFaceRadius,
                                  center.y - kFaceRadius,
                                  kFaceRadius * 2.0,
                                  kFaceRadius * 2.0);
    NSBezierPath *face = [NSBezierPath bezierPathWithOvalInRect:face_rect];
    NSGradient *face_gradient = [[NSGradient alloc]
        initWithStartingColor:make_color(kKnobTopColor)
                  endingColor:make_color(kKnobBottomColor)];
    [face_gradient drawInBezierPath:face angle:90.0];
    [make_color(kMutedColor) setStroke];
    face.lineWidth = 1.5;
    [face stroke];

    NSBezierPath *indicator = [NSBezierPath bezierPath];
    [indicator moveToPoint:NSMakePoint(center.x + std::cos(angle) * 10.0,
                                       center.y + std::sin(angle) * 10.0)];
    [indicator lineToPoint:NSMakePoint(center.x + std::cos(angle) * 40.0,
                                       center.y + std::sin(angle) * 40.0)];
    indicator.lineWidth = 5.0;
    indicator.lineCapStyle = NSLineCapStyleRound;
    [make_color(kKnobIndicatorColor) setStroke];
    [indicator stroke];
}
}

@interface ShipmentMeterView : NSView {
@private
    const std::atomic<double> *meter_;
}
- (instancetype)initWithMeter:(const std::atomic<double> *)meter;
- (void)refreshMeter;
@end

@implementation ShipmentMeterView

- (instancetype)initWithMeter:(const std::atomic<double> *)meter {
    self = [super initWithFrame:NSMakeRect(0.0, 0.0, kWindowWidth, kWindowHeight)];
    if (self) {
        meter_ = meter;
        self.wantsLayer = YES;
        self.layer.backgroundColor = make_color(kBackgroundColor).CGColor;
    }
    return self;
}

- (BOOL)isOpaque {
    return YES;
}

- (void)refreshMeter {
    [self setNeedsDisplay:YES];
}

- (void)drawRect:(NSRect)dirtyRect {
    (void)dirtyRect;
    const NSRect bounds = self.bounds;
    NSColor *const background = make_color(kBackgroundColor);
    NSColor *const panel = make_color(kPanelColor);
    NSColor *const muted = make_color(kMutedColor);
    NSColor *const accent = make_color(kAccentColor);
    NSColor *const text = make_color(kTextColor);

    [background setFill];
    NSRectFill(bounds);
    [panel setFill];
    [[NSBezierPath bezierPathWithRoundedRect:NSInsetRect(bounds, 18.0, 18.0)
                                     xRadius:18.0
                                     yRadius:18.0] fill];

    draw_text(@"THE SHIPMENT EVALUATOR",
              NSMakeRect(30.0, 446.0, bounds.size.width - 60.0, 26.0),
              [NSFont boldSystemFontOfSize:19.0],
              text,
              NSTextAlignmentCenter);
    draw_text(@"A highly scientific, completely predictable assessment",
              NSMakeRect(30.0, 421.0, bounds.size.width - 60.0, 20.0),
              [NSFont systemFontOfSize:12.0],
              muted,
              NSTextAlignmentCenter);

    const double meter = clamp_evaluation_meter(meter_->load(std::memory_order_relaxed));
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
        const double angle = evaluation_meter_angle(static_cast<double>(tick));
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

        const CGFloat labelRadius = arcRadius + 12.0;
        const NSPoint labelPoint = NSMakePoint(center.x + std::cos(angle) * labelRadius - 12.0,
                                               center.y + std::sin(angle) * labelRadius - 8.0);
        draw_text([NSString stringWithFormat:@"%d", tick],
                  NSMakeRect(labelPoint.x, labelPoint.y, 24.0, 16.0),
                  [NSFont monospacedDigitSystemFontOfSize:11.0 weight:NSFontWeightMedium],
                  muted,
                  NSTextAlignmentCenter);
    }

    draw_amplifier_knob(center, evaluation_meter_angle(meter));

    draw_text([NSString stringWithFormat:@"%.1f", meter],
              NSMakeRect(center.x - 44.0, center.y - 105.0, 88.0, 32.0),
              [NSFont monospacedDigitSystemFontOfSize:23.0 weight:NSFontWeightSemibold],
              text,
              NSTextAlignmentCenter);

    if (evaluation_is_complete(meter)) {
        draw_text(@"♬ ♫ ♪ ♩  →  SHIP IT !  ←  ♩ ♪ ♫ ♬",
                  NSMakeRect(28.0, 91.0, bounds.size.width - 56.0, 28.0),
                  [NSFont boldSystemFontOfSize:18.0],
                  accent,
                  NSTextAlignmentCenter);
        draw_text(@"A fixed result. Audio passes through unchanged.",
                  NSMakeRect(30.0, 66.0, bounds.size.width - 60.0, 17.0),
                  [NSFont systemFontOfSize:10.0],
                  muted,
                  NSTextAlignmentCenter);
    } else {
        draw_text(@"Start playback to begin the evaluation",
                  NSMakeRect(28.0, 91.0, bounds.size.width - 56.0, 24.0),
                  [NSFont systemFontOfSize:14.0],
                  muted,
                  NSTextAlignmentCenter);
    }
}
@end

ShipmentEvaluatorGui::ShipmentEvaluatorGui(const std::atomic<double> *meter)
    : meter_(meter) {}

ShipmentEvaluatorGui::~ShipmentEvaluatorGui() {
    destroy();
}

bool ShipmentEvaluatorGui::create(bool is_floating) {
    if (view_ || window_)
        return true;

    ShipmentMeterView *view =
        [[ShipmentMeterView alloc] initWithMeter:meter_];
    if (!view)
        return false;
    view_ = (__bridge_retained void *)view;

    if (is_floating) {
        NSWindow *window = [[NSWindow alloc]
            initWithContentRect:NSMakeRect(0.0, 0.0, kWindowWidth, kWindowHeight)
                      styleMask:NSWindowStyleMaskTitled |
                                NSWindowStyleMaskClosable |
                                NSWindowStyleMaskMiniaturizable
                        backing:NSBackingStoreBuffered
                          defer:NO];
        if (!window) {
            destroy();
            return false;
        }
        [window setContentView:view];
        window_ = (__bridge_retained void *)window;
    }

    return true;
}

void ShipmentEvaluatorGui::destroy() {
    if (window_) {
        NSWindow *window = (__bridge_transfer NSWindow *)window_;
        window_ = nullptr;
        [window setContentView:nil];
        [window close];
    }

    if (view_) {
        ShipmentMeterView *view = (__bridge_transfer ShipmentMeterView *)view_;
        view_ = nullptr;
        [view removeFromSuperview];
    }
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
    if (!view_ || window_ || !parent_view)
        return false;

    ShipmentMeterView *view = (__bridge ShipmentMeterView *)view_;
    NSView *parent = (__bridge NSView *)parent_view;
    [view removeFromSuperview];
    view.frame = parent.bounds;
    view.autoresizingMask = NSViewWidthSizable | NSViewHeightSizable;
    [parent addSubview:view];
    return true;
}

bool ShipmentEvaluatorGui::set_transient(void *parent_view) {
    if (!window_ || !parent_view)
        return false;

    NSView *parent = (__bridge NSView *)parent_view;
    NSWindow *parent_window = parent.window;
    if (!parent_window)
        return false;

    NSWindow *window = (__bridge NSWindow *)window_;
    [window setParentWindow:parent_window];
    return true;
}

void ShipmentEvaluatorGui::suggest_title(const char *title) {
    if (!window_ || !title)
        return;

    NSString *window_title = [NSString stringWithUTF8String:title];
    if (window_title)
        [(__bridge NSWindow *)window_ setTitle:window_title];
}

void ShipmentEvaluatorGui::refresh() {
    if (!view_)
        return;

    [(__bridge ShipmentMeterView *)view_ refreshMeter];
}

bool ShipmentEvaluatorGui::show() {
    if (!view_)
        return false;

    if (window_) {
        [(__bridge NSWindow *)window_ makeKeyAndOrderFront:nil];
        return true;
    }

    ShipmentMeterView *view = (__bridge ShipmentMeterView *)view_;
    view.hidden = NO;
    return true;
}

bool ShipmentEvaluatorGui::hide() {
    if (!view_)
        return false;

    if (window_) {
        [(__bridge NSWindow *)window_ orderOut:nil];
        return true;
    }

    ShipmentMeterView *view = (__bridge ShipmentMeterView *)view_;
    view.hidden = YES;
    return true;
}
