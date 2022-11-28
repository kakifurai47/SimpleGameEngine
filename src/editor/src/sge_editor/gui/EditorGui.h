#pragma once


#include <imgui.h>

#include "EditorGuiUitil.h"


namespace sge {



	struct PropertyAttribute { //<== put this in sge_reflection
		const char* name = "";
		bool hasRange = false;
		float min = 0.0f;
		float max = 0.0f;
	};

	class EditorGui 
	{
		using Util	   = EditorGuiUtil;
		using PropAttr = PropertyAttribute;
	public:

		class Window : public NonCopyable
		{
		public:
			Window(StrView name, bool* p_open = nullptr, ImGuiWindowFlags flags = 0) 
			{
				Begin(name, p_open, flags);
			}

			~Window() { ImGui::End(); }
		};

		class TreeNode : public NonCopyable
		{
		public:
			TreeNode(StrView label, size_t idx, ImGuiTreeNodeFlags flags = 0) {
				m_isOpen = ImGui::TreeNodeEx((void*)(intptr_t)idx, flags, TempString(label).c_str());
			}

			~TreeNode() {
				if(m_isOpen) ImGui::TreePop();
			}

			bool isOpen() const { return m_isOpen; }


		private:
			bool m_isOpen = false;
		};



		static bool Begin	(StrView name, bool* p_open, ImGuiWindowFlags flags = ImGuiWindowFlags_MenuBar)	
		{
			return ImGui::Begin(TempString(name).c_str(), p_open, flags);
		}

		static void End		()	{ ImGui::End();								}
		static void NewFrame()	{ ImGui::NewFrame();						}
		static void Render	()	{ ImGui::Render();							}

		static void UpdatePlatformWindows		() { ImGui::UpdatePlatformWindows();		}
		static void RenderPlatformWindowsDefault() { ImGui::RenderPlatformWindowsDefault(); }

		static ImGuiViewport* FindViewportByID(ImGuiID id) { return ImGui::FindViewportByID(id); }

		static ImGuiIO&			GetIO			() { return ImGui::GetIO();				}
		static ImGuiPlatformIO& GetPlatformIO	() { return ImGui::GetPlatformIO();		}
		static ImGuiViewport*	GetMainViewport	() { return ImGui::GetMainViewport();	}
		static ImDrawData*		GetDrawData		() { return ImGui::GetDrawData();		}

		template<class T> static void PushId(T id) { ImGui::PushID(id); }
						  static void PopId ()     { ImGui::PopID();    }


		static void				CreateContext		();

		static ImGuiContext*	GetCurrentContext	() { return ImGui::GetCurrentContext(); }
		static void				DestroyContext		() { return ImGui::DestroyContext();	}

		static bool IsItemClicked() { return ImGui::IsItemClicked(); }


		static void ShowDemoWindow(bool* open) { ImGui::ShowDemoWindow(open); }

		static bool Button(const char* label, const Vec2f& size = { 0,0 }) { return ImGui::Button(label, Util::toImVec2(size)); }

		template<class FMT_STR, class... ARGs>
		static void Text(FMT_STR&& str, ARGs&&... args)
		{
			TempString temp;
			FmtTo(temp, str, args...);
			ImGui::Text(temp.data ());
		}

		static void Text(StrView str) { TempString temp(str); Text(temp.c_str()); }

		inline static bool Drag(const char* label, i32&	  value, float speed = 1.0f, f32 min = 0,    f32 max = 0)    { return ImGui::DragInt   (label, &value,     speed, (int)min, (int)max); }//temp
		inline static bool Drag(const char* label, Vec2i& value, float speed = 1.0f, f32 min = 0,    f32 max = 0)    { return ImGui::DragInt2  (label, value.data, speed, (int)min, (int)max); }//temp
		inline static bool Drag(const char* label, Vec3i& value, float speed = 1.0f, f32 min = 0,    f32 max = 0)    { return ImGui::DragInt3  (label, value.data, speed, (int)min, (int)max); }//temp
		inline static bool Drag(const char* label, f32&	  value, float speed = 1.0f, f32 min = 0.0f, f32 max = 0.0f) { return ImGui::DragFloat (label, &value,     speed,	   min,		 max); }
		inline static bool Drag(const char* label, Vec2f& value, float speed = 1.0f, f32 min = 0.0f, f32 max = 0.0f) { return ImGui::DragFloat2(label, value.data, speed,	   min,		 max); }
		inline static bool Drag(const char* label, Vec3f& value, float speed = 1.0f, f32 min = 0.0f, f32 max = 0.0f) { return ImGui::DragFloat3(label, value.data, speed,	   min,		 max); }
		
		inline static bool Slider(const char* label, i32&	value, f32 min, f32 max) { return ImGui::SliderInt   (label, &value,	 (int)min, (int)max); }
		inline static bool Slider(const char* label, Vec2i&	value, f32 min, f32 max) { return ImGui::SliderInt   (label, value.data, (int)min, (int)max); }
		inline static bool Slider(const char* label, Vec3i&	value, f32 min, f32 max) { return ImGui::SliderInt   (label, value.data, (int)min, (int)max); }
		inline static bool Slider(const char* label, f32&	value, f32 min, f32 max) { return ImGui::SliderFloat (label, &value,		  min,		max); }
		inline static bool Slider(const char* label, Vec2f&	value, f32 min, f32 max) { return ImGui::SliderFloat2(label, value.data,	  min,		max); }
		inline static bool Slider(const char* label, Vec3f&	value, f32 min, f32 max) { return ImGui::SliderFloat3(label, value.data,	  min,		max); }

		template<class T> inline static bool ShowNumbers(T& value, const PropAttr& att) {
			if (att.hasRange) { return Slider(att.name, value,		 att.min, att.max); }
			else			  { return Drag  (att.name, value, 1.0f, att.min, att.max); }
		}

		inline static bool ShowRotation(Quat4f& value, const PropAttr& att)
		{
			auto deg = Math::degrees( value.euler() );

			PropAttr att_;

			//TODO : cache euler to have valid conversion from quat -> euler

			att_.name = att.name; // <- temp
			att_.hasRange = true;
			att_.min =   0.f;
			att_.max = 89.0f;

			if (ShowNumbers(deg, att_)) {
				value = Quat4f::s_euler( Math::radians( deg ) );
				return true;
			}
			return false;
		}

		inline static bool ShowColor(Color4f& value, const PropAttr& att)
		{
			return ImGui::ColorEdit4(att.name, value.data);
		}



		template<class T> inline static bool ShowProperty(T&		value, const PropAttr& att = {})	{ return Property<T>::Show(value, att); }
		template<>		  inline static bool ShowProperty(i32&		value, const PropAttr& att)			{ return ShowNumbers (value, att);		}
		template<>		  inline static bool ShowProperty(f32&		value, const PropAttr& att)			{ return ShowNumbers (value, att);		}
		template<>		  inline static bool ShowProperty(Vec3f&	value, const PropAttr& att)			{ return ShowNumbers (value, att);		}
		template<>		  inline static bool ShowProperty(Quat4f&	value, const PropAttr& att)			{ return ShowRotation(value, att);		}
		template<>		  inline static bool ShowProperty(Color4f&	value, const PropAttr& att)			{ return ShowColor   (value, att);		}

		template<class T>
		static bool ShowFieldProperty(Object& object, const FieldInfo& info, const PropAttr& att)
		{
			auto& oldValue = info.getValue<T>(&object);
			auto  newValue = oldValue;

			if (ShowProperty(newValue, att)) {
				info.setValue<T>(&object, newValue);
				return true;
			}
			return false;
		}

		template<class T, class ENABLE = void> struct Property {};

		template<class T>
		struct Property<T, std::enable_if_t<std::is_base_of_v<Object, T>>> 
		{
			static bool Show(T& obj, const PropAttr& att)
			{
				auto* info = obj.typeInfo();

				int i = 0;

				bool changed = false;

				for (auto& field : info->fields()) 
				{
					auto*  fieldType = field.type;

					PropAttr fieldAtt;
					fieldAtt.name = field.name;
					fieldAtt.hasRange  = false;

					i++;

					if (fieldType == sge_typeof<i32>   ()) { PushId(i); ShowFieldProperty<i32>   (obj, field, fieldAtt); PopId(); continue; }
					if (fieldType == sge_typeof<f32>   ()) { PushId(i); ShowFieldProperty<f32>   (obj, field, fieldAtt); PopId(); continue; }
					if (fieldType == sge_typeof<Vec3f> ()) { PushId(i); ShowFieldProperty<Vec3f> (obj, field, fieldAtt); PopId(); continue; }
					if (fieldType == sge_typeof<Quat4f>()) { PushId(i); ShowFieldProperty<Quat4f>(obj, field, fieldAtt); PopId(); continue; }

					if (fieldType -> isKindOf  <Object>()) { 
						PushId(i);
						ShowProperty(obj.memberValue<Object>(field), fieldAtt);
						PopId();
						continue; 
					}

					SGE_ASSERT("EditorGui : Unsupported format", false);
				}

				return changed; //temp
			}
		};

		template<class T>
		struct Property<T, std::enable_if_t< meta::is_vector<T>::value || meta::is_span<T>::value >>
		{
			static bool Show(T& container, const PropAttr& att)
			{
				auto flag = ImGuiTreeNodeFlags_OpenOnArrow |
							ImGuiTreeNodeFlags_OpenOnDoubleClick |
							ImGuiTreeNodeFlags_SpanAvailWidth;

				bool changed = false;

				for (size_t i = 0; i < container.size(); i++)
				{
					auto* item = meta::to_ptr(container[i]);
					auto* info = item->typeInfo();

					EditorGui::TreeNode node(info->name, i, flag);

					if (node.isOpen()) {
						EditorGui::ShowProperty(*item);
					}
				}
				return changed;
			}
		};







	};



}