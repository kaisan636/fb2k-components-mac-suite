//
//  SpectrumView.h
//  foo_jl_spectrum_mac
//
//  Core Graphics view that renders frequency bars with falling peak caps.
//

#pragma once

#import <Cocoa/Cocoa.h>

NS_ASSUME_NONNULL_BEGIN

@class SpectrumView;

@protocol SpectrumViewDelegate <NSObject>
- (void)spectrumViewRequestsContextMenu:(SpectrumView *)view atPoint:(NSPoint)point;
@optional
// Sent on resize while the bar count follows the view ("Auto" bars, or Curve
// mode), with the count for the new size.
- (void)spectrumView:(SpectrumView *)view autoBarCountChanged:(NSInteger)count;
// Sent on a left double-click.
- (void)spectrumViewRequestsFullScreenToggle:(SpectrumView *)view;
// Sent when the view enters or leaves a window (view.window is already updated).
- (void)spectrumViewDidMoveToWindow:(SpectrumView *)view;
@end

@interface SpectrumView : NSView

@property (nonatomic, weak, nullable) id<SpectrumViewDelegate> delegate;

// Whether audio is currently being displayed (affects the idle placeholder).
@property (nonatomic) BOOL playing;

// Re-read display settings (colors, style, gap, peak hold) from config.
- (void)reloadSettings;

// Bar count that follows the view size: in Curve mode one curve point per
// kCurvePointPitch points along the frequency axis, otherwise ("Auto" bars)
// one bar per kAutoBarPitch points.
- (NSInteger)autoBarCount;

// Sample rate of the audio shown. The analyzer stops at half of it, and so do
// the frequency grid, labels and readout.
- (void)setSampleRate:(double)rate;

// Provide the latest bar magnitudes, shadow fill, peak positions and, in
// Mid + side mode, the side channel (nil otherwise); each 0..1 with `count`
// entries. Redraws.
- (void)setBarsData:(const float *)bars
             shadow:(const float *)shadow
              peaks:(const float *)peaks
               side:(const float * _Nullable)side
              count:(NSInteger)count;

// SPL readout overlay (drawn top-left when shown). RMS, AVG and held-peak SPL
// already include the calibration offset and playback volume; truePeakDb and
// rmsDbFS are raw dBFS readings. warnSPL (>= 50) colours the line amber at
// that SPL and red 5 dB above it.
- (void)setSplReadout:(BOOL)show
                rmsSPL:(float)rmsSPL
                avgSPL:(float)avgSPL
               peakSPL:(float)peakSPL
            truePeakDb:(float)truePeakDb
               rmsDbFS:(float)rmsDbFS
               warnSPL:(float)warnSPL;

@end

NS_ASSUME_NONNULL_END
