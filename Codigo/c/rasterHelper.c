#include "rasterHelper.h"

bool interpolate1ON = true;

void ProcessScanLine_m3(Vec3 color, Vertex* va, Vertex* vb, Vertex* vc, Vertex* vd, SDL_Surface* sf, float* depthBuffer, ScanLineData* data,  SDL_Surface* tex)
{
    
    Vec3 pa, pb, pc, pd;
    CopyVec(pa, va->coordinates, 3);
    CopyVec(pb, vb->coordinates, 3);
    CopyVec(pc, vc->coordinates, 3);
    CopyVec(pd, vd->coordinates, 3);

    float gradient1 = pa[1] != pb[1] ? (data->currentY - pa[1]) / (pb[1] - pa[1]) : 1;
    float gradient2 = pc[1] != pd[1] ? (data->currentY - pc[1]) / (pd[1] - pc[1]) : 1;
            
    int sx = (int)Interpolate(pa[0], pb[0], gradient1, interpolate1ON); 
    int ex = (int)Interpolate(pc[0], pd[0], gradient2, interpolate1ON);

    float z1 = Interpolate(pa[2], pb[2], gradient1, interpolate1ON);
    float z2 = Interpolate(pc[2], pd[2], gradient2, interpolate1ON);

    float snl = Interpolate(data->ndotla, data->ndotlb, gradient1, interpolate1ON);
    float enl = Interpolate(data->ndotlc, data->ndotld, gradient2, interpolate1ON);

    float su = Interpolate(va->texCoordinates[0], vb->texCoordinates[0], gradient1, interpolate1ON);
    float eu = Interpolate(vc->texCoordinates[0], vd->texCoordinates[0], gradient2, interpolate1ON);
    float sv = Interpolate(va->texCoordinates[1], vb->texCoordinates[1], gradient1, interpolate1ON);
    float ev = Interpolate(vc->texCoordinates[1], vd->texCoordinates[1], gradient2, interpolate1ON);

    int x;
    for (x = sx ; x < ex; x++)
    {
  		float gradient = (x - sx) / (float)(ex - sx);
        float z = Interpolate(z1, z2, gradient, interpolate1ON);
        float ndotl = Interpolate(snl, enl, gradient, interpolate1ON);

        float u = Interpolate(su, eu, gradient, interpolate1ON);
        float v = Interpolate(sv, ev, gradient, interpolate1ON);

        Uint32 textureColor;
        Vec3 texColor;

        if (tex != NULL)
        {
            textureColor = Map(tex, u, v);
            Uint32ToVec4(textureColor, texColor);
            VecByVec(texColor, color, texColor, 3);
        }
        else
        {    
            texColor[0] = color[0]; texColor[1] = color[1]; texColor[2] = color[2];     //si no hay textura entonces pongo  color blanco 
        }

        VecByScalar(texColor, ndotl, texColor, 3);
        textureColor = Vec4ToUint32P(texColor[2], texColor[1], texColor[0]);
        putpixel(x, data->currentY, z, depthBuffer, sf, textureColor);
    }
        
}

void DrawTriangle_m3(Vec3 color, Vertex* p1, Vertex* p2, Vertex* p3, SDL_Surface* sf, float* depthBuffer,  SDL_Surface* tex, Vec3 lightPos)
{
    
    float nl1 = ComputeNDotL(p1->wcoordinates, p1->normal, lightPos);
    float nl2 = ComputeNDotL(p2->wcoordinates, p2->normal, lightPos);
    float nl3 = ComputeNDotL(p3->wcoordinates, p3->normal, lightPos);

    // inverse slopes
    float dP1P2, dP1P3;

    // Computing inverse slopes
    if (p2->coordinates[1] - p1->coordinates[1] > 0)
    	dP1P2 = (p2->coordinates[0] - p1->coordinates[0]) / (p2->coordinates[1] - p1->coordinates[1]);//cheque la pendiente 
    else
    	dP1P2 = 0;

    if (p3->coordinates[1] - p1->coordinates[1] > 0)
    	dP1P3 = (p3->coordinates[0] - p1->coordinates[0]) / (p3->coordinates[1] - p1->coordinates[1]);
    else
    	dP1P3 = 0;

    
    if(p1->coordinates[1] != p2->coordinates[1])
    {  
    	if (dP1P2 > dP1P3)
    	{
        	ScanLineData data;
            int y;
        	for ( y = (int)p1->coordinates[1]; y <= (int)p3->coordinates[1]; y++)
        	{
                data.currentY = y;

        		if (y < p2->coordinates[1])
                {
                    data.ndotla = nl1;
                    data.ndotlb = nl3;
                    data.ndotlc = nl1;
                    data.ndotld = nl2;
                    ProcessScanLine_m3(color, p1, p3, p1, p2, sf, depthBuffer, &data, tex);
                }    
            	else
            	{
                    data.ndotla = nl1;
                    data.ndotlb = nl3;
                    data.ndotlc = nl2;
                    data.ndotld = nl3;
                    ProcessScanLine_m3(color, p1, p3, p2, p3, sf, depthBuffer, &data, tex);
                }	
        	}
		}
    	else
    	{
        	ScanLineData data;
            int y;
        	for (y = (int)p1->coordinates[1]; y <= (int)p3->coordinates[1]; y++)
        	{
                data.currentY = y;

            	if (y < p2->coordinates[1])
        		{
                    data.ndotla = nl1;
                    data.ndotlb = nl2;
                    data.ndotlc = nl1;
                    data.ndotld = nl3;
                    ProcessScanLine_m3(color, p1, p2, p1, p3, sf, depthBuffer, &data, tex);
                }
            	else
        		{
                    data.ndotla = nl2;
                    data.ndotlb = nl3;
                    data.ndotlc = nl1;
                    data.ndotld = nl3;
                    ProcessScanLine_m3(color, p2, p3, p1, p3, sf, depthBuffer, &data, tex);
                }
        	}
    	}
    }
    else
    {
    	if (p1->coordinates[0] < p2->coordinates[0])
    	{
        	ScanLineData data;
            int y;
        	for ( y = (int)p1->coordinates[1]; y <= (int)p3->coordinates[1]; y++)
        	{
                data.currentY = y;

                data.ndotla = nl1;
                data.ndotlb = nl3;
                data.ndotlc = nl2;
                data.ndotld = nl3;
                ProcessScanLine_m3(color, p1, p3, p2, p3, sf, depthBuffer, &data, tex);
                
        	}
        }
    	else
    	{
        	ScanLineData data;
            int y;
        	for (y = (int)p1->coordinates[1]; y <= (int)p3->coordinates[1]; y++)
        	{
                data.currentY = y;

                data.ndotla = nl2;
                data.ndotlb = nl3;
                data.ndotlc = nl1;
                data.ndotld = nl3;
                ProcessScanLine_m3(color, p2, p3, p1, p3, sf, depthBuffer, &data, tex);
        	}
        }
    } 
}


void ProcessScanLine_m2(Vec3 color, Vertex* va, Vertex* vb, Vertex* vc, Vertex* vd, SDL_Surface* sf, float* depthBuffer, ScanLineData* data,  SDL_Surface* tex)
{

    Vec3 pa, pb, pc, pd;
    CopyVec(pa, va->coordinates, 3);
    CopyVec(pb, vb->coordinates, 3);
    CopyVec(pc, vc->coordinates, 3);
    CopyVec(pd, vd->coordinates, 3);

    float gradient1 = pa[1] != pb[1] ? (data->currentY - pa[1]) / (pb[1] - pa[1]) : 1;
    float gradient2 = pc[1] != pd[1] ? (data->currentY - pc[1]) / (pd[1] - pc[1]) : 1;
            
    int sx = (int)Interpolate(pa[0], pb[0], gradient1, interpolate1ON);
    int ex = (int)Interpolate(pc[0], pd[0], gradient2, interpolate1ON);

    float z1 = Interpolate(pa[2], pb[2], gradient1, interpolate1ON);
    float z2 = Interpolate(pc[2], pd[2], gradient2, interpolate1ON);

    float su = Interpolate(va->texCoordinates[0], vb->texCoordinates[0], gradient1, interpolate1ON);
    float eu = Interpolate(vc->texCoordinates[0], vd->texCoordinates[0], gradient2, interpolate1ON);
    float sv = Interpolate(va->texCoordinates[1], vb->texCoordinates[1], gradient1, interpolate1ON);
    float ev = Interpolate(vc->texCoordinates[1], vd->texCoordinates[1], gradient2, interpolate1ON);

     int x;
    for (x = sx ; x < ex; x++)
    {
        float gradient = (x - sx) / (float)(ex - sx);
        float z = Interpolate(z1, z2, gradient, interpolate1ON);

        float u = Interpolate(su, eu, gradient, interpolate1ON);
        float v = Interpolate(sv, ev, gradient, interpolate1ON);

        Uint32 textureColor;
        Vec3 texColor;

        if (tex != NULL)
        {
            textureColor = Map(tex, u, v);
            Uint32ToVec4(textureColor, texColor);
            VecByVec(texColor, color, texColor, 3);
        }
        else
        {    
            texColor[0] = color[0]; texColor[1] = color[1]; texColor[2] = color[2];     //si no hay textura entonces pongo  color blanco 
        }
        
        textureColor = Vec4ToUint32P(texColor[2], texColor[1], texColor[0]);
        putpixel(x, data->currentY, z, depthBuffer, sf, textureColor);
    }   
}

void DrawTriangle_m2(Vec3 color, Vertex* p1, Vertex* p2, Vertex* p3, SDL_Surface* sf, float* depthBuffer,  SDL_Surface* tex)
{
    // inverse slopes
    float dP1P2, dP1P3;

    // Computing inverse slopes
    if (p2->coordinates[1] - p1->coordinates[1] > 0)
        dP1P2 = (p2->coordinates[0] - p1->coordinates[0]) / (p2->coordinates[1] - p1->coordinates[1]);//cheque la pendiente 
    else
        dP1P2 = 0;

    if (p3->coordinates[1] - p1->coordinates[1] > 0)
        dP1P3 = (p3->coordinates[0] - p1->coordinates[0]) / (p3->coordinates[1] - p1->coordinates[1]);
    else
        dP1P3 = 0;

    
    if(p1->coordinates[1] != p2->coordinates[1])
    {  
        if (dP1P2 > dP1P3)
        {
            ScanLineData data;
            int y;
            for ( y = (int)p1->coordinates[1]; y <= (int)p3->coordinates[1]; y++)
            {
                data.currentY = y;

                if (y < p2->coordinates[1])
                {
                    ProcessScanLine_m2(color, p1, p3, p1, p2, sf, depthBuffer, &data, tex);
                }    
                else
                {
                    ProcessScanLine_m2(color, p1, p3, p2, p3, sf, depthBuffer, &data, tex);
                }   
            }
        }
        else
        {
            ScanLineData data;
            int y;
            for (y = (int)p1->coordinates[1]; y <= (int)p3->coordinates[1]; y++)
            {
                data.currentY = y;

                if (y < p2->coordinates[1])
                {
                    ProcessScanLine_m2(color, p1, p2, p1, p3, sf, depthBuffer, &data, tex);
                }
                else
                {
                    ProcessScanLine_m2(color, p2, p3, p1, p3, sf, depthBuffer, &data, tex);
                }
            }
        }
    }
    else
    {
        if (p1->coordinates[0] < p2->coordinates[0])
        {
            ScanLineData data;
            int y;
            for ( y = (int)p1->coordinates[1]; y <= (int)p3->coordinates[1]; y++)
            {
                data.currentY = y;

                ProcessScanLine_m2(color, p1, p3, p2, p3, sf, depthBuffer, &data, tex);
            }
        }
        else
        {
            ScanLineData data;
            int y;
            for (y = (int)p1->coordinates[1]; y <= (int)p3->coordinates[1]; y++)
            {
                data.currentY = y;

                ProcessScanLine_m2(color, p2, p3, p1, p3, sf, depthBuffer, &data, tex);
            }
        }
    } 
}

void ProcessScanLine_m1(Vec3 va, Vec3 vb, Vec3 vc, Vec3 vd, SDL_Surface* sf, int Y)
{
    float gradient1 = va[1] != vb[1] ? (Y - va[1]) / (vb[1] - va[1]) : 1;
    float gradient2 = vc[1] != vd[1] ? (Y - vc[1]) / (vd[1] - vc[1]) : 1;
            
    int sx = (int)Interpolate(va[0], vb[0], gradient1, interpolate1ON);
    int ex = (int)Interpolate(vc[0], vd[0], gradient2, interpolate1ON);

    Uint32 textureColor = 0xfffffff;      // color por default blanco

    putpixel_m1(sx, Y , sf, textureColor);
    putpixel_m1(ex, Y, sf, textureColor);  
}

void DrawTriangle_m1(Vec3 p1, Vec3 p2, Vec3 p3, SDL_Surface* sf)
{
    Vec3 temp;
    Vec2 tempt;
    if (p1[1] > p2[1])
    {
        CopyVec(temp, p2, 3);
        CopyVec(p2, p1, 3);
        CopyVec(p1, temp, 3);
    }

    if (p2[1] > p3[1])
    {
        CopyVec(temp, p2, 3);
        CopyVec(p2, p3, 3);
        CopyVec(p3, temp, 3);
    }

    if (p1[1] > p2[1])
    {
        CopyVec(temp, p2, 3);
        CopyVec(p2, p1, 3);
        CopyVec(p1, temp, 3);
    }

    // inverse slopes
    float dP1P2, dP1P3;

    // Computing inverse slopes
    if (p2[1] - p1[1] > 0)
        dP1P2 = (p2[0] - p1[0]) / (p2[1] - p1[1]);//cheque la pendiente 
    else
        dP1P2 = 0;

    if (p3[1] - p1[1] > 0)
        dP1P3 = (p3[0] - p1[0]) / (p3[1] - p1[1]);
    else
        dP1P3 = 0;


    if(p1[1] != p2[1])
    {  
        if (dP1P2 > dP1P3)
        {            
            int y;
            for ( y = (int)p1[1]; y <= (int)p3[1]; y++)
            {
                
                if (y < p2[1])
                {
                    ProcessScanLine_m1(p1, p3, p1, p2, sf, y);
                }    
                else
                {
                    ProcessScanLine_m1(p1, p3, p2, p3, sf, y);
                }   
            }
        }
        else
        {            
            int y;
            for (y = (int)p1[1]; y <= (int)p3[1]; y++)
            {
                if (y < p2[1])
                {
                    ProcessScanLine_m1(p1, p2, p1, p3, sf, y);
                }
                else
                {
                    ProcessScanLine_m1(p2, p3, p1, p3, sf, y);
                }
            }
        }
    }
    else
    {
        if (p1[0] < p2[0])
        {
            int y;
            for ( y = (int)p1[1]; y <= (int)p3[1]; y++)
            {
                ProcessScanLine_m1(p1, p3, p2, p3, sf, y);
            }
        }
        else
        {
            int y;
            for (y = (int)p1[1]; y <= (int)p3[1]; y++)
            {
                ProcessScanLine_m1(p2, p3, p1, p3, sf, y);
            }
        }
    } 
}