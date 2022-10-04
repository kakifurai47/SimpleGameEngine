#pragma once


#include <imgui.h>

#include "EditorGuiUitil.h"


namespace sge {

	struct PropertyAttribute { //<== put this in sge_reflection
		const char* name = "";
		bool hasRange = false;
	};


	class EditorGui 
	{
		using Util	   = EditorGuiUtil;
		using PropAttr = PropertyAttribute;
	public:

		static bool Begin	(const char* name, bool* open)	{ return ImGui::Begin(name, open);	}
		static void End		()								{ ImGui::End();						}
		static void NewFrame()								{ ImGui::NewFrame();				}
		static void Render	()								{ ImGui::Render();					}

		static void UpdatePlatformWindows		() { ImGui::UpdatePlatformWindows();		}
		static void RenderPlatformWindowsDefault() { ImGui::RenderPlatformWindowsDefault(); }

		static ImGuiViewport* FindViewportByID(ImGuiID id) { return ImGui::FindViewportByID(id); }

		static ImGuiIO&			GetIO			() { return ImGui::GetIO();				}
		static ImGuiPlatformIO& GetPlatformIO	() { return ImGui::GetPlatformIO();		}
		static ImGuiViewport*	GetMainViewport	() { return ImGui::GetMainViewport();	}
		static ImDrawData*		GetDrawData		() { return ImGui::GetDrawData();		}

		template<class T> static void PushId(T id) { ImGui::PushID(id); }
						  static void PopId ()     { ImGui::PopID();    }


		static void				CreateContext		() { ImGui::CreateContext();			}
		static ImGuiContext*	GetCurrentContext	() { return ImGui::GetCurrentContext(); }
		static void				DestroyContext		() { return ImGui::DestroyContext();	}

		static void ShowDemoWindow(bool* open) { ImGui::ShowDemoWindow(open); }

		static bool Button(const char* label, const Vec2f& size = { 0,0 }) { return ImGui::Button(label, Util::toImVec2(size)); }


		template<class FMT_STR, class... ARGs>
		static void Text(FMT_STR&& str, ARGs&&... args)
		{
			TempString temp;
			FmtTo(temp, str, args...);
			ImGui::Text(temp.data ());
		}

		template<class T> inline static void Drag		(const char* label, T&		value);
		template<>		  inline static void Drag<i32>	(const char* label, i32&	value) { ImGui::DragInt	  (label, &value);	   }
		template<>		  inline static void Drag<f32>	(const char* label, f32&	value) { ImGui::DragFloat (label, &value);	   }
		template<>		  inline static void Drag<Vec3f>(const char* label, Vec3f&	value) { ImGui::DragFloat3(label, value.data); }

		template<class T> inline static void Slider		  (const char* label, T&		value);
		template<>		  inline static void Slider<i32>  (const char* label, i32&		value) {ImGui::SliderInt   (label, &value,		0, 1); }
		template<>		  inline static void Slider<f32>  (const char* label, f32&		value) {ImGui::SliderFloat (label, &value,		0, 1); }
		template<>		  inline static void Slider<Vec3f>(const char* label, Vec3f&	value) {ImGui::SliderFloat3(label, value.data,	0, 1); }

		template<class T> inline static void ShowNumbers(T& value, const PropAttr& att) {
			if (att.hasRange) { Slider(att.name, value); }
			else			  { Drag  (att.name, value); }
		}

		template<class T> inline static void ShowProperty(T&     value, const PropAttr& att = {})	{ Property<T>::Show(value, att); }
		template<>		  inline static void ShowProperty(i32&   value, const PropAttr& att)		{ ShowNumbers(value, att);		 }
		template<>		  inline static void ShowProperty(f32&   value, const PropAttr& att)		{ ShowNumbers(value, att);		 }
		template<>		  inline static void ShowProperty(Vec3f& value, const PropAttr& att)		{ ShowNumbers(value, att);		 }

		template<class T, class ENABLE = void> struct Property {};

		template<class T>
		struct Property<T, std::enable_if_t<std::is_base_of_v<Object, T>>> 
		{
			static void Show(T& obj, const PropAttr& att)
			{
				auto* info = obj.typeInfo();

				int i = 0;
				
				for (auto& field : info->fields()) 
				{
					auto*  fieldType = field.type;

					PropAttr fieldAtt;
					fieldAtt.name = field.name;
					fieldAtt.hasRange  =  true;

					i++;

					if (fieldType == sge_typeof<i32>  ()) { PushId(i); ShowProperty(obj.memberValue<i32>   (field), fieldAtt); PopId(); continue; }
					if (fieldType == sge_typeof<f32>  ()) { PushId(i); ShowProperty(obj.memberValue<f32>   (field), fieldAtt); PopId(); continue; }
					if (fieldType == sge_typeof<Vec3f>()) { PushId(i); ShowProperty(obj.memberValue<Vec3f> (field), fieldAtt); PopId(); continue; }
					if (fieldType->isKindOf<Object>   ()) { PushId(i); ShowProperty(obj.memberValue<Object>(field), fieldAtt); PopId(); continue; }


					SGE_ASSERT(false);
				}
			}
		};
	};
}