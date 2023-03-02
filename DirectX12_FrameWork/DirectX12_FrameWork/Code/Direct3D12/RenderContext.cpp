/*****************************************************************//**
 * \file   RenderContext.cpp
 * \brief  レンダリングコンテキスト
 * 
 * \author Hiroto Kuge
 * \date   March 2023
 *********************************************************************/
#include "RenderContext.h"
#include "RenderTarget.h"


void RenderContext::SetDescriptorHeap(DescriptorHeap& descHeap){
	m_descriptorHeaps[0] = descHeap.Get();
	m_pCommandList->SetDescriptorHeaps(1, m_descriptorHeaps);

	//ディスクリプタテーブルに登録する。
	if (descHeap.IsRegistConstantBuffer()) {
		SetGraphicsRootDescriptorTable(0, descHeap.GetConstantBufferGpuDescriptorStartHandle());
	}
	if (descHeap.IsRegistShaderResource()) {
		SetGraphicsRootDescriptorTable(1, descHeap.GetShaderResourceGpuDescriptorStartHandle());
	}
	if (descHeap.IsRegistUavResource()) {
		SetGraphicsRootDescriptorTable(2, descHeap.GetUavResourceGpuDescriptorStartHandle());
	}
}

void RenderContext::SetComputeDescriptorHeap(DescriptorHeap& descHeap){
	m_descriptorHeaps[0] = descHeap.Get();
	m_pCommandList->SetDescriptorHeaps(1, m_descriptorHeaps);

	//ディスクリプタテーブルに登録する。
	if (descHeap.IsRegistConstantBuffer()) {
		SetComputeRootDescriptorTable(0, descHeap.GetConstantBufferGpuDescriptorStartHandle());
	}
	if (descHeap.IsRegistShaderResource()) {
		SetComputeRootDescriptorTable(1, descHeap.GetShaderResourceGpuDescriptorStartHandle());
	}
	if (descHeap.IsRegistUavResource()) {
		SetComputeRootDescriptorTable(2, descHeap.GetUavResourceGpuDescriptorStartHandle());
	}
}

void RenderContext::WaitUntilFinishDrawingToRenderTarget(RenderTarget& renderTarget){
	WaitUntilFinishDrawingToRenderTarget(renderTarget.GetRenderTargetTexture().Get());
}
 void RenderContext::WaitUntilToPossibleSetRenderTarget(RenderTarget& renderTarget)
{
	WaitUntilToPossibleSetRenderTarget(renderTarget.GetRenderTargetTexture().Get());
}
 void RenderContext::WaitUntilToPossibleSetRenderTargets(int numRt, RenderTarget* renderTargets[])
{
	for (int i = 0; i < numRt; i++) {
		WaitUntilToPossibleSetRenderTarget(*renderTargets[i]);
	}
}
 void RenderContext::WaitUntilFinishDrawingToRenderTargets(int numRt, RenderTarget* renderTargets[])
{
	for (int i = 0; i < numRt; i++) {
		WaitUntilFinishDrawingToRenderTarget(*renderTargets[i]);
	}
}
void RenderContext::SetRenderTargets(UINT numRT, RenderTarget* renderTargets[]){
	//d
	D3D12_CPU_DESCRIPTOR_HANDLE rtDSHandleTbl[32];
	int rtNo = 0;
	for (UINT rtNo = 0; rtNo < numRT; rtNo++) {
		rtDSHandleTbl[rtNo] = renderTargets[rtNo]->GetRTVCpuDescriptorHandle();
	}
	if (renderTargets[0]->IsExsitDepthStencilBuffer()) {
		//深度バッファがある。
		D3D12_CPU_DESCRIPTOR_HANDLE dsDS = renderTargets[0]->GetDSVCpuDescriptorHandle();
		m_pCommandList->OMSetRenderTargets(numRT, rtDSHandleTbl, FALSE, &dsDS);
	}
	else {
		//深度バッファがない。
		m_pCommandList->OMSetRenderTargets(numRT, rtDSHandleTbl, FALSE, nullptr);
	}

}

void RenderContext::SetRenderTargetAndViewport(RenderTarget& renderTarget){
	D3D12_VIEWPORT viewport;
	viewport.TopLeftX = 0;
	viewport.TopLeftY = 0;
	viewport.Width = static_cast<float>(renderTarget.GetWidth());
	viewport.Height = static_cast<float>(renderTarget.GetHeight());
	viewport.MinDepth = D3D12_MIN_DEPTH;
	viewport.MaxDepth = D3D12_MAX_DEPTH;
	SetViewportAndScissor(viewport);

	SetRenderTarget(renderTarget);
}

void RenderContext::SetRenderTargetsAndViewport(UINT numRT, RenderTarget* renderTargets[]){
	D3D12_VIEWPORT viewport;
	viewport.TopLeftX = 0;
	viewport.TopLeftY = 0;
	viewport.Width = static_cast<float>(renderTargets[0]->GetWidth());
	viewport.Height = static_cast<float>(renderTargets[0]->GetHeight());
	viewport.MinDepth = D3D12_MIN_DEPTH;
	viewport.MaxDepth = D3D12_MAX_DEPTH;
	SetViewportAndScissor(viewport);
	SetRenderTargets(numRT, renderTargets);
}

void RenderContext::ClearRenderTargetViews(int numRt, RenderTarget* renderTargets[]){
	if (renderTargets[0]->IsExsitDepthStencilBuffer()) {
		//深度バッファがある。
		ClearDepthStencilView(renderTargets[0]->GetDSVCpuDescriptorHandle(), renderTargets[0]->GetDSVClearValue());
	}
	for (int i = 0; i < numRt; i++) {
		ClearRenderTargetView(renderTargets[i]->GetRTVCpuDescriptorHandle(), renderTargets[i]->GetRTVClearColor());
	}
}
