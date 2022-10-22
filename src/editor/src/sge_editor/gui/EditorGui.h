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

		inline static void ShowColor(Color4f& value, const PropAttr& att)
		{
			ImGui::ColorEdit4(att.name, value.data);
		}
		



		template<class T> inline static void ShowProperty(T&		value, const PropAttr& att = {})	{ Property<T>::Show(value, att); }
		template<>		  inline static void ShowProperty(i32&		value, const PropAttr& att)			{ ShowNumbers(value, att);		 }
		template<>		  inline static void ShowProperty(f32&		value, const PropAttr& att)			{ ShowNumbers(value, att);		 }
		template<>		  inline static void ShowProperty(Vec3f&	value, const PropAttr& att)			{ ShowNumbers(value, att);		 }
		template<>		  inline static void ShowProperty(Color4f&	value, const PropAttr& att)			{ ShowColor  (value, att);		 }


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

					if (fieldType == sge_typeof<i32>   ()) { PushId(i); ShowProperty(obj.memberValue<i32>	(field), fieldAtt); PopId(); continue; }
					if (fieldType == sge_typeof<f32>   ()) { PushId(i); ShowProperty(obj.memberValue<f32>	(field), fieldAtt); PopId(); continue; }
					if (fieldType == sge_typeof<Vec3f> ()) { PushId(i); ShowProperty(obj.memberValue<Vec3f>	(field), fieldAtt); PopId(); continue; }
					if (fieldType -> isKindOf  <Object>()) { PushId(i); ShowProperty(obj.memberValue<Object>(field), fieldAtt); PopId(); continue; }

					SGE_ASSERT("EditorGui : Unsupported format", false);
				}
			}
		};

		template<class T>
		struct Property<T, std::enable_if_t< meta::is_vector<T>::value || meta::is_span<T>::value >>
		{
			static void Show(T& container, const PropAttr& att)
			{
				auto flag = ImGuiTreeNodeFlags_OpenOnArrow |
							ImGuiTreeNodeFlags_OpenOnDoubleClick |
							ImGuiTreeNodeFlags_SpanAvailWidth;

				for (size_t i = 0; i < container.size(); i++)
				{
					auto* item = meta::to_ptr(container[i]);
					auto* info = item->typeInfo();

					EditorGui::TreeNode node(info->name, i, flag);

					if (node.isOpen()) {
						EditorGui::ShowProperty(*item);
					}
				}
			}
		};







	};



}