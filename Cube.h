#pragma once

#include <windows.h>
#include <d3d11.h>
#include <d3dx11.h>
#include <d3dcompiler.h>
#include <xnamath.h>
#include "resource.h"
#include "helpers.h"


class Cube
{
protected:

    ID3D11VertexShader*         m_pVertexShader;
    ID3D11PixelShader*          m_pPixelShader;
    ID3D11InputLayout*          m_pVertexLayout;
    ID3D11Buffer*               m_pVertexBuffer;
    ID3D11Buffer*               m_pIndexBuffer;
    ID3D11Buffer*               m_pCBNeverChanges;
    ID3D11Buffer*               m_pCBChangeOnResize;
    ID3D11Buffer*               m_pCBChangesEveryFrame;
    ID3D11ShaderResourceView*   m_pTextureRV;
    ID3D11SamplerState*         m_pSamplerLinear;

    XMMATRIX                    m_World;
    XMMATRIX                    m_View;
    XMMATRIX                    m_Projection;
    XMFLOAT4                    m_vMeshColor;


    float m_TEST;

    struct SimpleVertex
    {
        XMFLOAT3 Pos;
        XMFLOAT2 Tex;
    };

    struct CBNeverChanges
    {
        XMMATRIX mView;
    };

    struct CBChangeOnResize
    {
        XMMATRIX mProjection;
    };

    struct CBChangesEveryFrame
    {
        XMMATRIX mWorld;
        XMFLOAT4 vMeshColor;
    };

public:
    
    Cube()
    : m_pVertexShader(NULL)
    , m_pPixelShader (NULL)
    , m_pVertexLayout(NULL)
    , m_pVertexBuffer(NULL)
    , m_pIndexBuffer (NULL)
    , m_pCBNeverChanges(NULL)
    , m_pCBChangeOnResize(NULL)
    , m_pCBChangesEveryFrame(NULL)
    , m_pTextureRV(NULL)
    , m_pSamplerLinear(NULL)
    , m_vMeshColor( 0.7f, 0.7f, 0.7f, 1.0f )
    {
    }


    ~Cube()
    {
        Cleanup();
    }

    void Cleanup()
    {
        if( m_pSamplerLinear )       m_pSamplerLinear->Release();
        if( m_pTextureRV )           m_pTextureRV->Release();
        if( m_pCBNeverChanges )      m_pCBNeverChanges->Release();
        if( m_pCBChangeOnResize )    m_pCBChangeOnResize->Release();
        if( m_pCBChangesEveryFrame ) m_pCBChangesEveryFrame->Release();
    }

    HRESULT Initialize(ID3D11Device* pD3dDevice, ID3D11DeviceContext* pDeviceContext, float screenRatio)
    {
        m_TEST = 1.f;

        // Compile the vertex shader
        ID3DBlob* pVSBlob = NULL;
        HRESULT hr = CompileShaderFromFile( L"Tutorial07.fx", "VS", "vs_4_0", &pVSBlob );
        if( FAILED( hr ) )
        {
            MessageBox( NULL,
                L"The FX file cannot be compiled.  Please run this executable from the directory that contains the FX file.", L"Error", MB_OK );
            return hr;
        }

        // Create the vertex shader
        hr = pD3dDevice->CreateVertexShader( pVSBlob->GetBufferPointer(), pVSBlob->GetBufferSize(), NULL, &m_pVertexShader );
        if( FAILED( hr ) )
        {    
            pVSBlob->Release();
            return hr;
        }

        // Define the input layout
        D3D11_INPUT_ELEMENT_DESC layout[] =
        {
            { "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
            { "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0 },
        };
        UINT numElements = ARRAYSIZE( layout );

        // Create the input layout
        hr = pD3dDevice->CreateInputLayout( layout, numElements, pVSBlob->GetBufferPointer(),
            pVSBlob->GetBufferSize(), &m_pVertexLayout );
        pVSBlob->Release();
        if( FAILED( hr ) )
            return hr;


        // Set the input layout
        pDeviceContext->IASetInputLayout( m_pVertexLayout );

        // Compile the pixel shader
        ID3DBlob* pPSBlob = NULL;
        hr = CompileShaderFromFile( L"Tutorial07.fx", "PS", "ps_4_0", &pPSBlob );
        if( FAILED( hr ) )
        {
            MessageBox( NULL,
                L"The FX file cannot be compiled.  Please run this executable from the directory that contains the FX file.", L"Error", MB_OK );
            return hr;
        }

        // Create the pixel shader
        hr = pD3dDevice->CreatePixelShader( pPSBlob->GetBufferPointer(), pPSBlob->GetBufferSize(), NULL, &m_pPixelShader );
        pPSBlob->Release();
        if( FAILED( hr ) )
            return hr;

        // Create vertex buffer
        SimpleVertex vertices[] =
        {
            { XMFLOAT3( -1.0f,  1.0f, -1.0f ), XMFLOAT2( 0.0f, 0.0f ) },
            { XMFLOAT3(  1.0f,  1.0f, -1.0f ), XMFLOAT2( 1.0f, 0.0f ) },
            { XMFLOAT3(  1.0f,  1.0f,  1.0f ), XMFLOAT2( 1.0f, 1.0f ) },
            { XMFLOAT3( -1.0f,  1.0f,  1.0f ), XMFLOAT2( 0.0f, 1.0f ) },

            { XMFLOAT3( -1.0f, -1.0f, -1.0f ), XMFLOAT2( 0.0f, 0.0f ) },
            { XMFLOAT3(  1.0f, -1.0f, -1.0f ), XMFLOAT2( 1.0f, 0.0f ) },
            { XMFLOAT3(  1.0f, -1.0f,  1.0f ), XMFLOAT2( 1.0f, 1.0f ) },
            { XMFLOAT3( -1.0f, -1.0f,  1.0f ), XMFLOAT2( 0.0f, 1.0f ) },

            { XMFLOAT3( -1.0f, -1.0f,  1.0f ), XMFLOAT2( 0.0f, 0.0f ) },
            { XMFLOAT3( -1.0f, -1.0f, -1.0f ), XMFLOAT2( 1.0f, 0.0f ) },
            { XMFLOAT3( -1.0f,  1.0f, -1.0f ), XMFLOAT2( 1.0f, 1.0f ) },
            { XMFLOAT3( -1.0f,  1.0f,  1.0f ), XMFLOAT2( 0.0f, 1.0f ) },

            { XMFLOAT3(  1.0f, -1.0f,  1.0f ), XMFLOAT2( 0.0f, 0.0f ) },
            { XMFLOAT3(  1.0f, -1.0f, -1.0f ), XMFLOAT2( 1.0f, 0.0f ) },
            { XMFLOAT3(  1.0f,  1.0f, -1.0f ), XMFLOAT2( 1.0f, 1.0f ) },
            { XMFLOAT3(  1.0f,  1.0f,  1.0f ), XMFLOAT2( 0.0f, 1.0f ) },

            { XMFLOAT3( -1.0f, -1.0f, -1.0f ), XMFLOAT2( 0.0f, 0.0f ) },
            { XMFLOAT3(  1.0f, -1.0f, -1.0f ), XMFLOAT2( 1.0f, 0.0f ) },
            { XMFLOAT3(  1.0f,  1.0f, -1.0f ), XMFLOAT2( 1.0f, 1.0f ) },
            { XMFLOAT3( -1.0f,  1.0f, -1.0f ), XMFLOAT2( 0.0f, 1.0f ) },

            { XMFLOAT3( -1.0f, -1.0f,  1.0f ), XMFLOAT2( 0.0f, 0.0f ) },
            { XMFLOAT3(  1.0f, -1.0f,  1.0f ), XMFLOAT2( 1.0f, 0.0f ) },
            { XMFLOAT3(  1.0f,  1.0f,  1.0f ), XMFLOAT2( 1.0f, 1.0f ) },
            { XMFLOAT3( -1.0f,  1.0f,  1.0f ), XMFLOAT2( 0.0f, 1.0f ) },
        };

        D3D11_BUFFER_DESC bd;
        ZeroMemory( &bd, sizeof(bd) );
        bd.Usage = D3D11_USAGE_DEFAULT;
        bd.ByteWidth = sizeof( SimpleVertex ) * 24;
        bd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
        bd.CPUAccessFlags = 0;
        D3D11_SUBRESOURCE_DATA InitData;
        ZeroMemory( &InitData, sizeof(InitData) );
        InitData.pSysMem = vertices;
        hr = pD3dDevice->CreateBuffer( &bd, &InitData, &m_pVertexBuffer );
        if( FAILED( hr ) )
            return hr;

        // Set vertex buffer
        UINT stride = sizeof( SimpleVertex );
        UINT offset = 0;
        pDeviceContext->IASetVertexBuffers( 0, 1, &m_pVertexBuffer, &stride, &offset );

        // Create index buffer
        // Create vertex buffer
        WORD indices[] =
        {
            3,1,0,
            2,1,3,

            6,4,5,
            7,4,6,

            11,9,8,
            10,9,11,

            14,12,13,
            15,12,14,

            19,17,16,
            18,17,19,

            22,20,21,
            23,20,22
        };

        bd.Usage = D3D11_USAGE_DEFAULT;
        bd.ByteWidth = sizeof( WORD ) * 36;
        bd.BindFlags = D3D11_BIND_INDEX_BUFFER;
        bd.CPUAccessFlags = 0;
        InitData.pSysMem = indices;
        hr = pD3dDevice->CreateBuffer( &bd, &InitData, &m_pIndexBuffer );
        if( FAILED( hr ) )
            return hr;

        // Set index buffer
        pDeviceContext->IASetIndexBuffer( m_pIndexBuffer, DXGI_FORMAT_R16_UINT, 0 );

        // Set primitive topology
        pDeviceContext->IASetPrimitiveTopology( D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST );

        // Create the constant buffers
        bd.Usage = D3D11_USAGE_DEFAULT;
        bd.ByteWidth = sizeof(CBNeverChanges);
        bd.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
        bd.CPUAccessFlags = 0;
        hr = pD3dDevice->CreateBuffer( &bd, NULL, &m_pCBNeverChanges );
        if( FAILED( hr ) )
            return hr;

        bd.ByteWidth = sizeof(CBChangeOnResize);
        hr = pD3dDevice->CreateBuffer( &bd, NULL, &m_pCBChangeOnResize );
        if( FAILED( hr ) )
            return hr;

        bd.ByteWidth = sizeof(CBChangesEveryFrame);
        hr = pD3dDevice->CreateBuffer( &bd, NULL, &m_pCBChangesEveryFrame );
        if( FAILED( hr ) )
            return hr;

        // Load the Texture
        hr = D3DX11CreateShaderResourceViewFromFile( pD3dDevice, L"seafloor.dds", NULL, NULL, &m_pTextureRV, NULL );
        if( FAILED( hr ) )
            return hr;

        // Create the sample state
        D3D11_SAMPLER_DESC sampDesc;
        ZeroMemory( &sampDesc, sizeof(sampDesc) );
        sampDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
        sampDesc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
        sampDesc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
        sampDesc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
        sampDesc.ComparisonFunc = D3D11_COMPARISON_NEVER;
        sampDesc.MinLOD = 0;
        sampDesc.MaxLOD = D3D11_FLOAT32_MAX;
        hr = pD3dDevice->CreateSamplerState( &sampDesc, &m_pSamplerLinear );
        if( FAILED( hr ) )
            return hr;

        // Initialize the world matrices
        m_World.r[0] = XMVectorSet( 0.0f, 3.0f, -6.0f, 0.0f );
        //m_World = XMMatrixIdentity();

        // Initialize the view matrix
        XMVECTOR Eye = XMVectorSet( 0.0f, 3.0f, -6.0f, 0.0f );
        XMVECTOR At = XMVectorSet( 0.0f, 1.0f, 0.0f, 0.0f );
        XMVECTOR Up = XMVectorSet( 0.0f, 1.0f, 0.0f, 0.0f );
        m_View = XMMatrixLookAtLH( Eye, At, Up );

        CBNeverChanges cbNeverChanges;
        cbNeverChanges.mView = XMMatrixTranspose( m_View );
        pDeviceContext->UpdateSubresource( m_pCBNeverChanges, 0, NULL, &cbNeverChanges, 0, 0 );

        // Initialize the projection matrix
        m_Projection = XMMatrixPerspectiveFovLH( XM_PIDIV4, screenRatio, 0.01f, 100.0f );

        CBChangeOnResize cbChangesOnResize;
        cbChangesOnResize.mProjection = XMMatrixTranspose( m_Projection );
        pDeviceContext->UpdateSubresource( m_pCBChangeOnResize, 0, NULL, &cbChangesOnResize, 0, 0 );

        return hr;
    }


    void Update(ID3D11DeviceContext* pDeviceContext, float t)
    {
        // Rotate cube around the origin
        m_World = XMMatrixRotationY( t );

        // Modify the color
        m_vMeshColor.x = ( sinf( t * 1.0f ) + 1.0f ) * 0.5f;
        m_vMeshColor.y = ( cosf( t * 3.0f ) + 1.0f ) * 0.5f;
        m_vMeshColor.z = ( sinf( t * 5.0f ) + 1.0f ) * 0.5f;



        //
        // Update variables that change once per frame
        //
        CBChangesEveryFrame cb;
        cb.mWorld = XMMatrixTranspose( m_World );
        cb.vMeshColor = m_vMeshColor;
        pDeviceContext->UpdateSubresource( m_pCBChangesEveryFrame, 0, NULL, &cb, 0, 0 );

    }




    void Render(ID3D11DeviceContext* pDeviceContext)
    {
        //
        // Render the cube
        //
        pDeviceContext->VSSetShader( m_pVertexShader, NULL, 0 );
        pDeviceContext->VSSetConstantBuffers( 0, 1, &m_pCBNeverChanges );
        pDeviceContext->VSSetConstantBuffers( 1, 1, &m_pCBChangeOnResize );
        pDeviceContext->VSSetConstantBuffers( 2, 1, &m_pCBChangesEveryFrame );
        pDeviceContext->PSSetShader( m_pPixelShader, NULL, 0 );
        pDeviceContext->PSSetConstantBuffers( 2, 1, &m_pCBChangesEveryFrame );
        pDeviceContext->PSSetShaderResources( 0, 1, &m_pTextureRV );
        pDeviceContext->PSSetSamplers( 0, 1, &m_pSamplerLinear );
        pDeviceContext->DrawIndexed( 36, 0, 0 );
    }

};