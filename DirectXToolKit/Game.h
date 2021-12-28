#pragma once
#include "DeviceResources.h"
#include "StepTimer.h"
#include <array>
class Game final : public DX::IDeviceNotify // ���̻� Game���� �ڽ��� ������
{
private:
	std::unique_ptr<DX::DeviceResources>		m_deviceResources;

	DX::StepTimer								m_timer;

	//Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> m_texCat;
	//Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> m_texBug;

	std::unique_ptr<DirectX::SpriteBatch>			 m_spriteBatch;
	std::unique_ptr<DirectX::CommonStates>			 m_commonStates;

	int m_currentFrame{ 0 };
	double m_timeToNextFrame{ 0.1f };
	std::array<Microsoft::WRL::ComPtr<ID3D11ShaderResourceView>, 10> m_textures;

	std::unique_ptr<DirectX::Keyboard>			m_keyBoard;
	std::unique_ptr<DirectX::Mouse>				m_mouse;

public:
	Game() noexcept(false);
	~Game();

	Game(Game&&) = default; // �̵� ������
	Game& operator=(Game&&) = default;	// �̵� ���� 

	Game(Game const&) = delete;
	Game& operator=(Game const&) = delete; // ���� �̵�

	void Initialize(HWND window, int width, int height);

	void Tick();

	void OnDeviceLost() override;
	void OnDeviceRestored() override;

	void OnActivated();
	void OnDeActivated();
	void OnSuspending();
	void OnResuming();
	void OnWindowMoved();
	void OnWindowSizeChange(int width, int height);

	void GetDefaultSize(int& width, int& height) const noexcept;

private:
	void Update(DX::StepTimer const& timer);
	void Render();

	void Clear();

	void CreateDeviceDependentResources();
	void CreateWindowSizeDependentResources();
};

