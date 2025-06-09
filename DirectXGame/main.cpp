#include "GameScene.h"
#include "KamataEngine.h"
#include "Shader.h"
#include "RootSignature.h"
#include "PipelineState.h"
#include "VertexBuffer.h"
#include <Windows.h>

using namespace KamataEngine;

// インプットレイアウト、ブレンドステート、ラスタライザステート
// 引数として　空のpipelineState、RootSignature、頂点シェーダーvs、ピクセルシェーダーps　を参照で受け取る
void SetupPipelineState(PipelineState& pipelineState, RootSignature& rs, Shader& vs, Shader& ps) {
	// InputLayout
	D3D12_INPUT_ELEMENT_DESC inputElementDescs[1] = {};
	inputElementDescs[0].SemanticName = "POSITION";
	inputElementDescs[0].SemanticIndex = 0;
	inputElementDescs[0].Format = DXGI_FORMAT_R32G32B32A32_FLOAT;
	inputElementDescs[0].AlignedByteOffset = D3D12_APPEND_ALIGNED_ELEMENT;

	D3D12_INPUT_LAYOUT_DESC inputLayoutDesc{};
	inputLayoutDesc.pInputElementDescs = inputElementDescs;
	inputLayoutDesc.NumElements = _countof(inputElementDescs);

	// BlendState
	D3D12_BLEND_DESC blendDesc{};
	// すべての色要素を書き込む
	blendDesc.RenderTarget[0].RenderTargetWriteMask = D3D12_COLOR_WRITE_ENABLE_ALL;

	// RasterizerState
	D3D12_RASTERIZER_DESC rasterizerDesc{};
	// 裏面（反時計回り）をカリングする
	rasterizerDesc.CullMode = D3D12_CULL_MODE_BACK;
	// 塗りつぶしモードをソリッドにする（ワイヤーフレームなら、D3D12_FILL_MODE_WIREFRAME）
	rasterizerDesc.FillMode = D3D12_FILL_MODE_SOLID;

	// PSOの生成
	D3D12_GRAPHICS_PIPELINE_STATE_DESC graphicsPipelineStateDesc{};
	graphicsPipelineStateDesc.pRootSignature = rs.Get();
	graphicsPipelineStateDesc.InputLayout = inputLayoutDesc;
	graphicsPipelineStateDesc.VS = {vs.GetDxcBlob()->GetBufferPointer(), vs.GetDxcBlob()->GetBufferSize()};
	graphicsPipelineStateDesc.PS = {ps.GetDxcBlob()->GetBufferPointer(), ps.GetDxcBlob()->GetBufferSize()};
	graphicsPipelineStateDesc.BlendState = blendDesc;
	graphicsPipelineStateDesc.RasterizerState = rasterizerDesc;

	// 書き込むRTVの情報
	graphicsPipelineStateDesc.NumRenderTargets = 1;
	graphicsPipelineStateDesc.RTVFormats[0] = DXGI_FORMAT_R8G8B8A8_UNORM_SRGB;
	// 利用するトポロジのタイプ。三角形
	graphicsPipelineStateDesc.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;
	// どのように画面に色を打ち込むかの設定
	graphicsPipelineStateDesc.SampleDesc.Count = 1;
	graphicsPipelineStateDesc.SampleMask = D3D12_DEFAULT_SAMPLE_MASK;

	// 準備は整った。PSOを生成する
	pipelineState.Create(graphicsPipelineStateDesc);
};

// Windowsアプリでのエントリーポイント(main関数)
int WINAPI WinMain(_In_ HINSTANCE, _In_opt_ HINSTANCE, _In_ LPSTR, _In_ int) {

	// エンジンの初期化
	KamataEngine::Initialize(L"LE3D_14_フジワラ_リオ");

	// DirectXインスタンスの取得
	DirectXCommon* dxCommon = DirectXCommon::GetInstance();

	// DirectXCommonクラスが管理している、ウィンドウの幅と高さの値を取得
	int32_t w = dxCommon->GetBackBufferWidth();
	int32_t h = dxCommon->GetBackBufferHeight();
	DebugText::GetInstance()->ConsolePrintf(std::format("width: {},height: {}\n", w, h).c_str());

	// DirectXCommonクラスが管理している、コマンドリストの取得
	ID3D12GraphicsCommandList* commandList = dxCommon->GetCommandList();

	// RootSignature作成
	// 構造体にデータを用意する
	RootSignature rs;
	rs.Create();

	// 頂点シェーダの読み込みとコンパイル
	Shader vs;
	vs.LoadDxc(L"Resources/shaders/TestVS.hlsl", L"vs_6_0");
	assert(vs.GetDxcBlob() != nullptr);

	// ピクセルシェーダの読み込みとコンパイル
	Shader ps;
	ps.LoadDxc(L"Resources/shaders/TestPS.hlsl", L"ps_6_0");
	assert(ps.GetDxcBlob() != nullptr);

	//// PSOの生成
	PipelineState pipelineState;
	SetupPipelineState(pipelineState, rs, vs, ps);

	// VertexResourceの生成
	//D3D12_HEAP_PROPERTIES uploadHeapProperties{};
	//uploadHeapProperties.Type = D3D12_HEAP_TYPE_UPLOAD;
	//// 頂点リソースの設定
	//D3D12_RESOURCE_DESC vertexResourceDesc{};
	//vertexResourceDesc.Dimension = D3D12_RESOURCE_DIMENSION_BUFFER;
	//vertexResourceDesc.Width = sizeof(Vector4) * 3;
	//// バッファの場合はこれらは1にする決まり
	//vertexResourceDesc.Height = 1;
	//vertexResourceDesc.DepthOrArraySize = 1;
	//vertexResourceDesc.MipLevels = 1;
	//vertexResourceDesc.SampleDesc.Count = 1;
	//// バッファの場合はこれらにする決まり
	//vertexResourceDesc.Layout = D3D12_TEXTURE_LAYOUT_ROW_MAJOR;
	//// 実際に頂点リソースを生成する
	//ID3D12Resource* vertexResource = nullptr;
	//HRESULT hr = dxCommon->GetDevice()->CreateCommittedResource(&uploadHeapProperties, D3D12_HEAP_FLAG_NONE, &vertexResourceDesc, D3D12_RESOURCE_STATE_GENERIC_READ, nullptr, IID_PPV_ARGS(&vertexResource));
	//assert(SUCCEEDED(hr));

	//// VertexBufferViewのを作成する
	//D3D12_VERTEX_BUFFER_VIEW vertexBufferView{};
	//// リソースの先頭アドレスから使う
	//vertexBufferView.BufferLocation = vertexResource->GetGPUVirtualAddress();
	//// 使用するリソースのサイズは頂点3つ分のサイズ
	//vertexBufferView.SizeInBytes = sizeof(Vector4) * 3;
	//// 1つの頂点のサイズ
	//vertexBufferView.StrideInBytes = sizeof(Vector4);
	VertexBuffer vb;
	vb.Create(sizeof(Vector4) * 3, sizeof(Vector4));

	// 頂点リソースにデータを書き込む
	Vector4* vertexData = nullptr;
	vb.Get()->Map(0, nullptr, reinterpret_cast<void**>(&vertexData));
	vertexData[0] = Vector4(-0.5f, -0.5f, 0.0f, 1.0f);
	vertexData[1] = Vector4(0.0f, 0.5f, 0.0f, 1.0f);
	vertexData[2] = Vector4(0.5f, -0.5f, 0.0f, 1.0f);
	// 頂点リソースのマップを解除する
	//vertexResource->Unmap(0, nullptr);

	// ゲームシーンのインスタンスの生成
	GameScene* gameScene = new GameScene();
	// ゲームシーンの初期化
	gameScene->Initialize();

	// メインループ
	while (true) {

		// エンジンの更新
		if (KamataEngine::Update()) {
			break;
		}

		// ゲームシーンの更新
		gameScene->Update();

		// 描画開始
		dxCommon->PreDraw();

		// コマンドを積む
		commandList->SetGraphicsRootSignature(rs.Get());
		commandList->SetPipelineState(pipelineState.Get());
		commandList->IASetVertexBuffers(0, 1, vb.GetView());
		// トポロジの設定
		commandList->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
		// 頂点数、インデックス数、インデックスの開始位置、インデックスのオフセット
		commandList->DrawInstanced(3, 1, 0, 0);

		// ゲームシーンの描画
		gameScene->Draw();

		dxCommon->PostDraw();
	}

	// 解放処理
	//vertexResource->Release();
	

	// ゲームシーンの解放
	delete gameScene;
	// nullptrの代入
	gameScene = nullptr;

	// エンジンの終了処理
	KamataEngine::Finalize();

	return 0;
}
