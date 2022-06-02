
namespace sge {

	template<class SHADER>
	class ShaderManager : public NonCopyable {
		using This = ShaderManager<SHADER>;
	public:
		static  This* current();
		const SHADER* find(StrView filename);
	private:
		Map<u128, SHADER> m_table;
	};

	class Shader_DX11;
	using ShaderManager_DX11 = ShaderManager<Shader_DX11>;

}