
namespace sge {

	template<class SHADER>
	class ShaderManager : public NonCopyable {
		using This = ShaderManager<SHADER>;
	public:
		static  This* current();
		SHADER* find (StrView	 filename);
		void	erase(const u128&	  key);
	private:
		Map<u128, SHADER> m_shaders;
	};

	class Shader_DX11;
	using ShaderManager_DX11 = ShaderManager<Shader_DX11>;

}