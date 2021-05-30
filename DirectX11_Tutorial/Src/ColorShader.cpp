#include <string_view>
#include <fstream>
#include <filesystem>
#include "Graphics/ColorShader.h"
using namespace DirectX;

ColorShader::ColorShader()
    : m_pVertexShader(0)
    , m_pPixelShader(0)
    , m_pLayout(0)
    , m_pMatrixBuffer(0)
{
}

ColorShader::ColorShader(const ColorShader& kOther)
{
    m_pVertexShader = kOther.m_pVertexShader;
    m_pPixelShader = kOther.m_pPixelShader;
    m_pLayout = kOther.m_pLayout;
    m_pMatrixBuffer = kOther.m_pMatrixBuffer;
}

ColorShader::~ColorShader()
{
}

bool ColorShader::Initialize(ID3D11Device* pDevice, HWND hwnd)
{
    return InitializeShader(pDevice, hwnd, L"Src/Shaders/ColorVS.hlsl", L"Src/Shaders/ColorPS.hlsl");
}

void ColorShader::Shutdown()
{
    // Shutdown the vertex and pixel shaders as well as the related objects
    ShutdownShader();
}

bool ColorShader::Render(ID3D11DeviceContext* pDeviceContext, int indexCount, XMMATRIX worldMatrix, XMMATRIX viewMatrix, XMMATRIX projectionMatrix)
{
    // Set the shader parameters that it will use for rendering.
    if (!SetShaderParameters(pDeviceContext, worldMatrix, viewMatrix, projectionMatrix))
    {
        return false;
    }

    // Now render the prepared buffers with the shader.
    RenderShader(pDeviceContext, indexCount);

    return true;
}

// Loads the shader files and makes it usable to DirectX and the GPU.
// You will also see the setup of the layout and how the vertex buffer data is going to look on the graphics pipeline in the GPU. 
// The layout will need the match the VertexType in the modelclass.h file as well as the one defined in the color.vs file.
bool ColorShader::InitializeShader(ID3D11Device* pDevice, HWND hwnd, const WCHAR* pVertexShaderFile, const WCHAR* pPixelShaderFile)
{
    HRESULT result;
    ID3D10Blob* pErrorMsg;
    ID3D10Blob* pVertexShaderBuffer;
    ID3D10Blob* pPixelShaderBuffer;
    D3D11_INPUT_ELEMENT_DESC polygonLayout[2];
    uint32_t numElements;
    D3D11_BUFFER_DESC matrixBufferDesc;

    // Initialize the pointers this function will use to null.
    pErrorMsg = nullptr;
    pVertexShaderBuffer = nullptr;
    pPixelShaderBuffer = nullptr;

    // Here is where we compile the shader program into buffers.
    // We give the name of the shader file, the name of the shader, the shader version(5.0 in DirectX 11), and the buffer to compile the shader info.
    // If it fails compiling the shader it will put an error message inside the pErrorMsg, which we send to another function to write out the error.
    // If it still fails and there is no error message string, then it means it could not find the shader file in which case we pop up a dialog box saying so.

    // Compile the vertex shader code.
    result = D3DCompileFromFile(pVertexShaderFile, nullptr, nullptr, "ColorVertexShader", "vs_5_0", D3D10_SHADER_ENABLE_STRICTNESS, 0, &pVertexShaderBuffer, &pErrorMsg);
    if (FAILED(result))
    {
        if (pErrorMsg)
        {
            OutputShaderErrorMessage(pErrorMsg, hwnd, pVertexShaderFile);
        }
        else
        {
            MessageBox(hwnd, pVertexShaderFile, L"Missing Shader File", MB_OK);
        }

        return false;
    }

    // Compile the pixel shader code.
    result = D3DCompileFromFile(pPixelShaderFile, nullptr, nullptr, "ColorPixelShader", "ps_5_0", D3D10_SHADER_ENABLE_STRICTNESS, 0, &pPixelShaderBuffer, &pErrorMsg);
    if (FAILED(result))
    {
        if (pErrorMsg)
        {
            OutputShaderErrorMessage(pErrorMsg, hwnd, pPixelShaderFile);
        }
        else
        {
            MessageBox(hwnd, pPixelShaderFile, L"Missing Shader File", MB_OK);
        }

        return false;
    }

    // User compiled buffers to create the shader objects themselves.
    // Then use these pointers to interface with the vertex and pixel shader from this point forward.

    // Create the vertex shader from the buffer
    result = pDevice->CreateVertexShader(pVertexShaderBuffer->GetBufferPointer(), pVertexShaderBuffer->GetBufferSize(), nullptr, &m_pVertexShader);
    if (FAILED(result))
    {
        return false;
    }

    // Create the pixel shader from the buffer
    result = pDevice->CreatePixelShader(pPixelShaderBuffer->GetBufferPointer(), pPixelShaderBuffer->GetBufferSize(), nullptr, &m_pPixelShader);
    if (FAILED(result))
    {
        return false;
    }
 
    // Next step: Create the layout of the vertex data that will be processed by the shader.
    // We need to create bothe position and colro vector to specify the size of both
    // The semantic name allows the shader to determine the usage of this element of the layout.
    // Format.
    
    // AlignedByteOffSet : Indicates how the data is spaced in the buffer.
    //                     We are telling it the first 12 bytes are position and the next 16 bytes will be color.
    //                     AlignedByteOffSet shows where each element begins.
    //                     You can use D3D11_APPEND_ALIGNED_ELEMENT instead of placing your own values in AlignedByteOffset

    // Create the vertex input layout desc
    // This setup needs to match the VertexType structure in the Model and in the shader.
    polygonLayout[0].SemanticName = "POSITION";
    polygonLayout[0].SemanticIndex = 0;
    polygonLayout[0].Format = DXGI_FORMAT_R32G32B32_FLOAT;
    polygonLayout[0].InputSlot = 0;
    polygonLayout[0].AlignedByteOffset = 0;
    polygonLayout[0].InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;
    polygonLayout[0].InstanceDataStepRate = 0;

    polygonLayout[1].SemanticName = "COLOR";
    polygonLayout[1].SemanticIndex = 0;
    polygonLayout[1].Format = DXGI_FORMAT_R32G32B32A32_FLOAT;
    polygonLayout[1].InputSlot = 0;
    polygonLayout[1].AlignedByteOffset = D3D11_APPEND_ALIGNED_ELEMENT;
    polygonLayout[1].InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;
    polygonLayout[1].InstanceDataStepRate = 0;

    // Get a count of the elements in the layout
    numElements = sizeof(polygonLayout) / sizeof(polygonLayout[0]);

    // Create the vertex input layout.
    result = pDevice->CreateInputLayout(polygonLayout, numElements, pVertexShaderBuffer->GetBufferPointer(), pVertexShaderBuffer->GetBufferSize(), &m_pLayout);
    if (FAILED(result))
    {
        return false;
    }

    // Release the vertex shader buffer and pixel shader buffer since they are no longer needed.
    pVertexShaderBuffer->Release();
    pVertexShaderBuffer = nullptr;

    pPixelShaderBuffer->Release();
    pPixelShaderBuffer = nullptr;

    // Setup the description of the dynamic matrix constant buffer that is in the vertex shader.
    matrixBufferDesc.Usage = D3D11_USAGE_DYNAMIC;                   // Set to dynamic sine we will be updating it each frame.
    matrixBufferDesc.ByteWidth = sizeof(MatrixBuffer);
    matrixBufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;        // Indicates this buffer will be a constant buffer.
    matrixBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;       // Need to match up with the usage.
    matrixBufferDesc.MiscFlags = 0;
    matrixBufferDesc.StructureByteStride = 0;

    // Create the constant buffer pointer so we can access the vertex shader constant buffer from within this class.
    result = pDevice->CreateBuffer(&matrixBufferDesc, nullptr, &m_pMatrixBuffer);
    if (FAILED(result))
    {
        return false;
    }

    return true;
}

void ColorShader::ShutdownShader()
{
    // Release all four interfaces that were setup in the InitializeShader function.
    if (m_pMatrixBuffer)
    {
        m_pMatrixBuffer->Release();
        m_pMatrixBuffer = nullptr;
    }

    if (m_pLayout)
    {
        m_pLayout->Release();
        m_pLayout = nullptr;
    }
    
    if (m_pPixelShader)
    {
        m_pPixelShader->Release();
        m_pPixelShader = nullptr;
    }

    if (m_pVertexShader)
    {
        m_pVertexShader->Release();
        m_pVertexShader = nullptr;
    }
}

// Writes out error messages that are generating when compiling either vetex shaders or pixel shaders.
void ColorShader::OutputShaderErrorMessage(ID3D10Blob* pErrorMsg, HWND hwnd, const WCHAR* pShaderFileName)
{
    std::string_view compileErrors;
    uint64_t bufferSize;
    std::ofstream fout;

    // Get a pointer to the error message text buffer.
    compileErrors = static_cast<char*>(pErrorMsg->GetBufferPointer());

    // Get the length of the message.
    bufferSize = pErrorMsg->GetBufferSize();

    // Open a file to write the error message to.
    fout.open("shader-error.txt");

    // Write out the error message.
    for (uint64_t i = 0; i < bufferSize; ++i)
    {
        fout << compileErrors[i];
    }

    fout.close();

    pErrorMsg->Release();
    pErrorMsg = nullptr;

    // Pop a message up on the screen to notify the user to check the text file for compile errors.
    MessageBox(hwnd, L"Error compiling shader. Check shader-error.txt for message.", pShaderFileName, MB_OK);
}

// First function caleed in the Render function
// It is called before RenderShader function to ensure the shader parameters are setup corretly.
bool ColorShader::SetShaderParameters(ID3D11DeviceContext* pDeviceContext, XMMATRIX worldMatrix, XMMATRIX viewMatrix, XMMATRIX projectionMatrix)
{
    // Trnaspose the matrices to prepare them for the shader.
    worldMatrix = XMMatrixTranspose(worldMatrix);
    viewMatrix = XMMatrixTranspose(viewMatrix);
    projectionMatrix = XMMatrixTranspose(projectionMatrix);

    // Lock the constant buffer so it can be written to.
    D3D11_MAPPED_SUBRESOURCE mappedResource;
    HRESULT result(pDeviceContext->Map(m_pMatrixBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource));
    if (FAILED(result))
    {
        return false;
    }

    // Get a pointer to the data in the constant buffer.
    MatrixBuffer* pData(static_cast<MatrixBuffer*>(mappedResource.pData));

    // Copy the matrices into the constant buffer.
    pData->m_world = worldMatrix;
    pData->m_view = viewMatrix;
    pData->m_projection = projectionMatrix;

    // Unlock the constant buffer.
    pDeviceContext->Unmap(m_pMatrixBuffer, 0);

    /// Now, Set the updated matrix buffer in the HLSL vertex shader.

    // Set the position of the constant buffer in the vertex shader.
    uint32_t bufferNumber(0);

    // Finally set the constnat buffer in the vertex shader with the updated values.
    pDeviceContext->VSSetConstantBuffers(bufferNumber, 1, &m_pMatrixBuffer);
    
    return true;
}

// Second function called in the Render function.
void ColorShader::RenderShader(ID3D11DeviceContext* pDeviceContext, int indexCount)
{
    // Set the vertex input layout in the input assembler.
    // This lets teh GPU know the format of the data in the vertex buffer.
    pDeviceContext->IASetInputLayout(m_pLayout);

    // Set the vertex and pixel shaders that will be used to render this triangle.
    pDeviceContext->VSSetShader(m_pVertexShader, nullptr, 0);
    pDeviceContext->PSSetShader(m_pPixelShader, nullptr, 0);

    // Render the triangle.
    pDeviceContext->DrawIndexed(indexCount, 0, 0);
}
