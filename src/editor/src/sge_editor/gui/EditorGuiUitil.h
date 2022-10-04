#pragma  once 

namespace sge
{
	class  EditorGuiHandle;
	struct EditorGuiUtil
	{
		template<class T> static inline
		ByteSpan makeByteSpan(ImVector<T>& vec) {
			return spanCast<const u8, const T>(Span<const T>(vec.Data, vec.Size));
		}

		static inline
		int getGuiButton(UIMouseEventButton src) {
			switch (src) {
				case UIMouseEventButton::Left:		return ImGuiMouseButton_::ImGuiMouseButton_Left;
				case UIMouseEventButton::Right:		return ImGuiMouseButton_::ImGuiMouseButton_Right;
				case UIMouseEventButton::Middle:	return ImGuiMouseButton_::ImGuiMouseButton_Middle;

				default: throw SGE_ERROR("Unhandled mouse button pressed");
			}
		}

		static inline ImVec2 toImVec2(const Vec2f & i) { return ImVec2( i.x, i.y ); }
		static inline Vec2f  toVec2f (const ImVec2& i) { return Vec2f ( i.x, i.y ); }

		static inline void   convert(::ImGuiPlatformMonitor& o, const MonitorInfo& i)
		{
			o.MainPos	= toImVec2(i.rect.pos	  );
			o.MainSize	= toImVec2(i.rect.size	  );
			o.WorkPos	= toImVec2(i.workRect.pos );
			o.WorkSize	= toImVec2(i.workRect.size);
			o.DpiScale	= i.dpiScale;
		}
	};







}