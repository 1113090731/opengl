
#include <atlimage.h>
#include "texture.h"

GLuint loadTexture(const char *fileName)
{
	BITMAP bm;
	GLuint idTexture = 0;
	CImage img;             //需要头文件atlimage.h  
	CString s = fileName;
	LPCTSTR lpcFileName = s;
	HRESULT hr = img.Load(lpcFileName);

	if (!SUCCEEDED(hr))   //文件加载失败  
	{
		printf("文件加载失败%s", fileName);
		return NULL;
	}

	HBITMAP hbmp = img;
	if (!GetObject(hbmp, sizeof(bm), &bm))
		return 0;
	// 上下镜像才正确
	int width = bm.bmWidthBytes;
	int height = bm.bmHeight;
	for (int i = 0; i < width; ++i){
		for (int j = 0; j < height / 2; ++j){
			BYTE *buffer = (BYTE *)bm.bmBits;
			BYTE temp = buffer[j*width + i];
			buffer[j*width + i] = buffer[(height - j - 1)*width + i];
			buffer[(height - j - 1)*width + i] = temp;
		}
	}


	glGenTextures(1, &idTexture);
	if (idTexture)
	{
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, idTexture);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR_MIPMAP_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE); // 无接缝模式
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		glPixelStoref(GL_PACK_ALIGNMENT, 1);
		
		/*
		if (bm.bmBitsPixel == 32){
			glTexStorage2D(GL_TEXTURE_2D, 4, GL_RGBA, bm.bmWidth, bm.bmHeight);
			glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, bm.bmWidth, bm.bmHeight, GL_BGRA, GL_UNSIGNED_BYTE, bm.bmBits);
		}
		else{
			glTexStorage2D(GL_TEXTURE_2D, 4, GL_RGB, bm.bmWidth, bm.bmHeight);
			glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, bm.bmWidth, bm.bmHeight, GL_BGR, GL_UNSIGNED_BYTE, bm.bmBits); //这里不是GL_RGB  
		}
		*/
		
		if (bm.bmBitsPixel == 32){
			glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, bm.bmWidth, bm.bmHeight, 0, GL_BGRA, GL_UNSIGNED_BYTE, bm.bmBits);
		}
		else{
			glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, bm.bmWidth, bm.bmHeight, 0, GL_BGR, GL_UNSIGNED_BYTE, bm.bmBits); //这里不是GL_RGB  
		}
		
		glGenerateMipmap(GL_TEXTURE_2D);
	}
	return idTexture;
}

GLuint loadTextureCImage(CImage img){
	BITMAP bm;
	GLuint idTexture = 0;
	HBITMAP hbmp = img;

	if (!GetObject(hbmp, sizeof(bm), &bm))
		return 0;
	// 贴图上下镜像
	int width = bm.bmWidthBytes;
	int height = bm.bmHeight;
	for (int i = 0; i < width; ++i){
		for (int j = 0; j < height / 2; ++j){
			BYTE *buffer = (BYTE *)bm.bmBits;
			BYTE temp = buffer[j*width + i];
			buffer[j*width + i] = buffer[(height - j - 1)*width + i];
			buffer[(height - j - 1)*width + i] = temp;
		}
	}

	glGenTextures(1, &idTexture);
	if (idTexture)
	{
		glBindTexture(GL_TEXTURE_2D, idTexture);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, 0x812F); // 无接缝模式
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, 0x812F);
		glPixelStoref(GL_PACK_ALIGNMENT, 1);

		if (bm.bmBitsPixel == 32)
			glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, bm.bmWidth, bm.bmHeight, 0, GL_BGRA_EXT, GL_UNSIGNED_BYTE, bm.bmBits);
		else
			glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, bm.bmWidth, bm.bmHeight, 0, GL_BGR_EXT, GL_UNSIGNED_BYTE, bm.bmBits);
	}
	return idTexture;
}

GLuint  LoadMemImage(void *  pMemData, long  len)
{
	HGLOBAL hGlobal = GlobalAlloc(GMEM_MOVEABLE, len);
	void *  pData = GlobalLock(hGlobal);
	memcpy(pData, pMemData, len);
	GlobalUnlock(hGlobal);
	IStream *  pStream = NULL;
	CImage image;
	if (CreateStreamOnHGlobal(hGlobal, TRUE, &pStream) == S_OK)
	{

		if (SUCCEEDED(image.Load(pStream)))
		{
			return loadTextureCImage(image);
		}
		pStream->Release();
	}
	GlobalFree(hGlobal);
	return 0;
}

GLuint loadBlp(const char *file){

	FILE *Handle = fopen(file, "rb");
	DWORD temp;
	fseek(Handle, 23 * 4, SEEK_CUR);
	fread(&temp, 4, 1, Handle);
	DWORD size = temp;
	fseek(Handle, 15 * 4, SEEK_CUR);
	fread(&temp, 4, 1, Handle);
	DWORD headerSize = temp;
	BYTE *buffer = new BYTE[headerSize + size];
	fread(buffer, headerSize + size, 1, Handle);
	return LoadMemImage(buffer, headerSize + size);
}