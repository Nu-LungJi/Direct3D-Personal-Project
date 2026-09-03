#pragma once

#define	WINCX		1280.f
#define	WINCY		720.f

#define ADD_PRTOBJ(_SCNNUMB, _CMPTYPE, _CMP)	GameInstance::GetInstance().Get_ProtoManager()->Add_ProtoType(_SCNNUMB, _CMPTYPE, _CMP)

#define INVERTNODE(NODE)	make_unique<InverterNode>(BlackBoard, shared_from_this(), NODE)

#define LERP(LSTART, LEND, DT)	(LSTART + (LEND - LSTART) * DT)
#define EASEOUT(WEIGHT, POW)	(1.f - powf(1.f - WEIGHT, POW))
#define EASEIN(WEIGHT, POW)		(powf(WEIGHT, POW))
#define SMOOTHSTEP(WEIGHT)		(WEIGHT * WEIGHT * (3.f - 2.f * WEIGHT))