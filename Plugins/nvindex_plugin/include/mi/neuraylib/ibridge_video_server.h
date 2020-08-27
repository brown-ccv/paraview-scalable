//*****************************************************************************
// Copyright 2018 NVIDIA Corporation. All rights reserved.
//*****************************************************************************
/// \file
/// \brief  Bridge video handling
//*****************************************************************************

#ifndef MI_UNPUBLISHED_BRIDGE_VIDEO_SERVER_H
#define MI_UNPUBLISHED_BRIDGE_VIDEO_SERVER_H

#include <mi/base/interface_declare.h>
#include <mi/neuraylib/typedefs.h>

namespace mi
{

class IString;

namespace neuraylib
{
class IBuffer;
class ICanvas;
}

namespace bridge
{

class IVideo_source;

/** \addtogroup mi_neuray_bridge_server
@{
*/

/// Server-side video context that generates and encodes video frames for the corresponding
/// client-side video context.
///
/// An application using the video transmission facilities of the Bridge API must implement the
/// abstract interface #mi::bridge::IVideo_source and set it by calling #set_video_source() to
/// generate video frames.
///
/// Note that even though the focus is on video, it is equally valid to transmit any kind of data
/// suitable for streaming, and the application can deliver pure data buffers in addition to, or
/// instead of, video frames.
///
/// \see #mi::bridge::IServer_session::get_video_context(), #mi::bridge::IClient_video_context
class IServer_video_context : public mi::base::Interface_declare<0x9f1c260c, 0x43a7, 0x439e, 0x9a,
                                0x9f, 0xb0, 0xc0, 0x24, 0xc4, 0xdc, 0xbe>
{
public:
  /// Notifies the Bridge that the application produced the next video frame.
  ///
  /// When the process is ready to process the next frame it calls as response the callback
  /// #mi::bridge::IVideo_source::video_get_next_frame() of the video source associated with this
  /// video context (see also #set_video_source()).
  ///
  /// This method can be called any number of times between callbacks, but multiple calls after
  /// the last \c video_get_next_frame() callback will only result in a single callback of
  /// \c video_get_next_frame().
  virtual void frame_ready() = 0;

  /// Sets the video source that will produce video frames generated by the server.
  ///
  /// \param video_source   The video source to set. Replaces any previously set video source
  ///                       (if any). Pass \c NULL to disconnect the currently set video source.
  virtual void set_video_source(IVideo_source* video_source) = 0;

  /// Returns the currently set video source, or \c NULL if none is set.
  virtual IVideo_source* get_video_source() const = 0;

  /// Sets the video format for encoding.
  ///
  /// \note It is recommended to change settings like video format, frame rate, and bit rate only
  ///       from the #mi::bridge::IVideo_source::video_get_next_frame() callback (or before the
  ///       first call to #frame_ready()). Otherwise, it is not guaranteed that multiple changes
  ///       will be applied atomically to the next frame (because encoding of the next frame might
  ///       start at any time).
  ///
  /// \see #get_video_format(), #set_max_frame_rate(), #set_bit_rate()
  ///
  /// \param format   The video format to use when encoding canvases. Supported formats:
  ///                 - \c "h264": h264 encoding, requires an h264 encoder plugin.
  ///                 - \c "lossless": lossless encoding, supports all pixel formats, built-in.
  ///                 - any valid image format: any supported image format, requires the
  ///                   corresponding image plugin.
  /// \return
  ///                 -     0: Success.
  ///                 -    -1: Unsupported format.
  ///                 - <= -2: Unspecified error.
  virtual Sint32 set_video_format(const char* format) = 0;

  /// Returns the current video format for encoding.
  ///
  /// \see #set_video_format()
  virtual const IString* get_video_format() const = 0;

  /// Sets the maximum frame rate.
  ///
  /// This setting limits the rate at with which frames are sent from the server to this rate, but
  /// the frame rate is allowed to drop below this if frames are not produced or encoded quickly
  /// enough.
  ///
  /// The \c "h264" format will use the frame rate and the current bit rate when calculating the
  /// budget for each frame. The quality of the \c "lossless" format and image formats is not
  /// affected by this setting.
  ///
  /// \note It is recommended to change settings like video format, frame rate, and bit rate only
  ///       from the #mi::bridge::IVideo_source::video_get_next_frame() callback (or before the
  ///       first call to #frame_ready()). Otherwise, it is not guaranteed that multiple changes
  ///       will be applied atomically to the next frame (because encoding of the next frame might
  ///       start at any time).
  ///
  /// \see #get_max_frame_rate(), #set_video_format(), #set_bit_rate()
  ///
  /// \param frame_rate   The maximum frame rate to use.
  /// \return             0 in case of success, < 0 in case of failure.
  virtual Sint32 set_max_frame_rate(Uint32 frame_rate) = 0;

  /// Returns the maximum frame rate.
  ///
  /// \see set_max_frame_rate()
  virtual Uint32 get_max_frame_rate() const = 0;

  /// Sets the current frame rate. This is an optional hint that can be set to inform the video
  /// context about the actual current frame rate. It is recommended to call this every time the
  /// actual frame rate changes substantially, or simply call in each
  /// #mi::bridge::IVideo_source::video_get_next_frame() callback with the current value.
  ///
  /// \note It is recommended to change settings like video format, frame rate, and bit rate only
  ///       from the #mi::bridge::IVideo_source::video_get_next_frame() callback (or before the
  ///       first call to #frame_ready()). Otherwise, it is not guaranteed that multiple changes
  ///       will be applied atomically to the next frame (because encoding of the next frame might
  ///       start at any time).
  ///
  /// \see #get_max_frame_rate(), #set_video_format(), #set_bit_rate()
  ///
  /// \param frame_rate   The maximum frame rate to use.
  /// \return             0 in case of success, < 0 in case of failure.
  virtual Sint32 set_render_frame_rate(Float64 frame_rate) = 0;

  /// Sets the bit rate. This method is deprecated and is equivalent to calling set_max_bitrate()
  /// and set_min_bitrate() with the same value.
  ///
  /// The \c "h264" format will use the frame rate and the current bit rate when calculating the
  /// budget for each frame. The quality of the \c "lossless" format and image formats is not
  /// affected by this setting.
  ///
  /// \note It is recommended to change settings like video format, frame rate, and bit rate only
  ///       from the #mi::bridge::IVideo_source::video_get_next_frame() callback (or before the
  ///       first call to #frame_ready()). Otherwise, it is not guaranteed that multiple changes
  ///       will be applied atomically to the next frame (because encoding of the next frame might
  ///       start at any time).
  ///
  /// \see #get_bit_rate(), #set_video_format(), #set_max_frame_rate()
  ///
  /// \param bit_rate   The bit rate to use
  /// \return           0 in case of success, < 0 in case of failure.
  virtual Sint32 set_bit_rate(Uint32 bit_rate) = 0;

  /// Sets the maximum bitrate used by the video stream.
  ///
  /// Bridge video will attempt to automatically adjust the bitrate to adapt to the available
  /// bandwidth. The bitrate won't go below the set minimum bitrate. If the available bandwidth
  /// is smaller then frame rate will go down to compensate. Likewise, the bitrate won't go
  /// above the set maximum bitrate. Set min and max bitrate to the same value to override the
  /// automatic adaptation. The max bitrate setting takes precedence if max bitrate
  /// is set lower than min bitrate.
  ///
  /// The \c "h264" format will use the frame rate and the current bit rate when calculating the
  /// budget for each frame. The quality of the \c "lossless" format and image formats is not
  /// affected by this setting.
  ///
  /// \note It is recommended to change settings like video format, frame rate, and bit rate only
  ///       from the #mi::bridge::IVideo_source::video_get_next_frame() callback (or before the
  ///       first call to #frame_ready()). Otherwise, it is not guaranteed that multiple changes
  ///       will be applied atomically to the next frame (because encoding of the next frame might
  ///       start at any time).
  ///
  /// \see #get_max_bitrate(), #set_min_bitrate(), #get_bit_rate(), #set_video_format(),
  ///      #set_max_frame_rate()
  ///
  /// \param bit_rate   The bit rate to use
  /// \return           0 in case of success, < 0 in case of failure.
  virtual Sint32 set_max_bitrate(Uint32 bit_rate) = 0;

  /// Returns the currently set maximum bitrate.
  ///
  /// \see #set_max_bitrate()
  virtual Uint32 get_max_bitrate() const = 0;

  /// Sets the minimum bitrate used by the video stream.
  ///
  /// Bridge video will attempt to automatically adjust the bitrate to adapt to the available
  /// bandwidth. The bitrate won't go below the set minimum bitrate. If the available bandwidth
  /// is smaller then frame rate will go down to compensate. Likewise, the bitrate won't go
  /// above the set maximum bitrate. Set min and max bitrate to the same value to override the
  /// automatic adaptation. The max bitrate setting takes precedence if max bitrate
  /// is set lower than min bitrate.
  ///
  /// The \c "h264" format will use the frame rate and the current bit rate when calculating the
  /// budget for each frame. The quality of the \c "lossless" format and image formats is not
  /// affected by this setting.
  ///
  /// \note It is recommended to change settings like video format, frame rate, and bit rate only
  ///       from the #mi::bridge::IVideo_source::video_get_next_frame() callback (or before the
  ///       first call to #frame_ready()). Otherwise, it is not guaranteed that multiple changes
  ///       will be applied atomically to the next frame (because encoding of the next frame might
  ///       start at any time).
  ///
  /// \see #get_max_bitrate(), #set_min_bitrate(), #get_bit_rate(), #set_video_format(),
  ///      #set_max_frame_rate()
  ///
  /// \param bit_rate   The bit rate to use
  /// \return           0 in case of success, < 0 in case of failure.
  virtual Sint32 set_min_bitrate(Uint32 bit_rate) = 0;

  /// Returns the currently set minimum bitrate.
  ///
  /// \see #set_min_bitrate()
  virtual Uint32 get_min_bitrate() const = 0;

  /// Returns the currently used bit rate.
  ///
  /// \see #set_bit_rate()
  virtual Uint32 get_bit_rate() const = 0;

  /// Communicates a progress message to the client side.
  ///
  /// The Bridge video protocol invokes IVideo_sink::video_progress() on the video sink associated
  /// with the corresponding video context on the client side and passes the arguments provided
  /// here.
  ///
  /// \param value     The progress value.
  /// \param area      The progress area.
  /// \param message   The progress message.
  virtual void report_progress(Float64 value, const char* area, const char* message) = 0;

  /// Communicates an error message to the client side.
  ///
  /// The Bridge video protocol invokes IVideo_sink::video_error() on the video sink associated
  /// with the corresponding video context on the client side and passes the arguments provided
  /// here.
  ///
  /// \param error_code      Error code specified by the application.
  /// \param error_message   A short description of the error.
  virtual void report_error(Sint32 error_code, const char* error_message) = 0;

  /// Resets the video stream and any stateful video encoders like \c h264.
  virtual void reset() = 0;

  /// Closes the video stream associated with this context and frees all resources.
  ///
  /// This will also close the associated video context on the client.
  ///
  /// #set_video_source(), #mi::bridge::IClient_video_context
  virtual void close() = 0;

  /// Returns the ID of the video context.
  virtual Uint32 get_id() const = 0;
};

/// Abstract interface to generate video frames that are transmitted to the corresponding
/// client-side video context.
///
/// \see #mi::bridge::IServer_video_context::set_video_source()
class IVideo_source : public mi::base::Interface_declare<0x972224a4, 0xa63b, 0x42ce, 0x96, 0xfe,
                        0xe1, 0x33, 0xf9, 0x81, 0x1c, 0x64>
{
public:
  /// Returns data for the next frame.
  ///
  /// The Bridge video protocol calls this method in response to a call to
  /// #mi::bridge::IServer_video_context::frame_ready().
  ///
  /// The \p data buffer can contain data associated with the canvas, or, if it suits the
  /// application more, it can be sent instead of a canvas. The raw data buffer will be delivered
  /// to the client as is (not encoded or compressed in any way) and it is up to the application
  /// to define the format of the data.
  ///
  /// \note The content of \c *frame and \c *data must not be modified until the next call of
  ///       this method.
  ///
  /// \param[out] frame   The optional frame to send. \p *frame can be \c NULL if only data is to
  ///                     be sent. If both \p *frame and \p *data are \c NULL, nothing will be
  ///                     sent.
  /// \param[out] data    The optional data to send. \p *data can be \c NULL if only a frame is to
  ///                     be sent. If both \p *frame and \p *data are \c NULL, nothing will be
  ///                     sent.
  /// \return             0 in case of success, < 0 in case of failure.
  virtual Sint32 video_get_next_frame(neuraylib::ICanvas** frame, neuraylib::IBuffer** data) = 0;

  /// Called if the video context failed to encode or send the last frame delivered by a
  /// call to #video_get_next_frame().
  ///
  /// \param error_code
  ///                     - -1: Invalid canvas. The canvas has unsupported features, or is not
  ///                           compatible with current settings.
  ///                     - -3: Network error. Lost connection to the client.
  ///                     - -4: Encoding error. Failed to encode the frame.
  /// \param message      A short description of the error.
  virtual void video_error(Sint32 error_code, const char* message) = 0;

  /// Called when the video context has been closed.
  ///
  /// \param reason
  ///                        -  0: Closed by the server.
  ///                        -  1: Closed by the client.
  ///                        - -1: Network error.
  virtual void video_context_closed(Sint32 reason) = 0;
};

/*@}*/ // end group mi_neuray_bridge_server

} // namespace bridge

} // namespace mi

#endif // MI_UNPUBLISHED_BRIDGE_VIDEO_SERVER_H