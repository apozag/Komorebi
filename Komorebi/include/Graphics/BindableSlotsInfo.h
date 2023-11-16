#pragma once

#define MAX_DIRLIGHTS   5
#define MAX_SPOTLIGHTS  5
#define MAX_POINTLIGHTS 5

#define PCBUFF_GLOBAL_SLOT            0
#define PCBUFF_CAMERATRANSFORM_SLOT		1
#define PCBUFF_LIGHTINFO_SLOT	        2
#define PCBUFF_LIGHTTRANSFORM_SLOT	  3
#define PCBUFF_SHADOW_SLOT		        4
#define PCBUFF_FREE_SLOT		          5

#define VCBUFF_GLOBAL_SLOT            0
#define VCBUFF_CAMERATRANSFORM_SLOT		1
#define VCBUFF_MODEL_SLOT		          2
#define VCBUFF_BONES_SLOT		          3
#define VCBUFF_LIGHTTRANSFORM_SLOT	  4
#define VCBUFF_FREE_SLOT		          5

#define GCBUFF_GLOBAL_SLOT            0
#define GCBUFF_CAMERATRANSFORM_SLOT   1
#define GCBUFF_FREE_SLOT              2

#define SRV_SHADOWMAP_SLOT	0
#define SRV_PCF_SLOT			  6
#define SRV_FREE_SLOT			  7