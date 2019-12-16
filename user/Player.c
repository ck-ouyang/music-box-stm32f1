#include "stm32f10x.h"
#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include "SynthCore.h"
#include "Player.h"

extern unsigned char Score0[];
extern unsigned char Score1[];
extern unsigned char Score2[];
extern unsigned char Score3[];
extern unsigned char Score4[];
extern unsigned char Score5[];
extern unsigned char Score6[];
extern unsigned char Score7[];

unsigned char *ScoreSelected;

void Player32kProc(Player *player)
{
    SynthAsm(&(player->mainSynthesizer));
    player->currentTick++;
    if(player->decayGenTick<200)
         player->decayGenTick+=1;
}

void PlayerProcess(Player *player)
{

    uint8_t temp;

    if (player->decayGenTick >= 150)
    {
        GenDecayEnvlopeAsm(&(player->mainSynthesizer));
        player->decayGenTick = 0;
    }
    if (player->status == STATUS_PLAYING)
    {
        if ((player->currentTick >> 8) >= player->lastScoreTick)
        {
            do
            {
                temp = *(player->scorePointer);
                player->scorePointer++;
                if (temp == 0xFF)
                {
                    player->status = STATUS_STOP;
                }
                else
                {
                    NoteOnAsm(&(player->mainSynthesizer), temp - 3);
                }
            } while ((temp & 0x80) == 0);
            
            PlayUpdateNextScoreTick(player);
        }
    }
}

void PlayUpdateNextScoreTick(Player *player)
{
    uint32_t tempU32;
    uint8_t temp;
    tempU32 = player->lastScoreTick;
    do
    {
        temp = *(player->scorePointer);
        player->scorePointer++;
        tempU32 += temp;
    } while (temp == 0xFF);
    player->lastScoreTick = tempU32;
}

void SelectScore(){
	switch((GPIOB -> IDR >> 3 & 0b111)){
		case 0: ScoreSelected = Score0; break;
		case 1: ScoreSelected = Score1; break;
		case 2: ScoreSelected = Score2; break;
		case 3: ScoreSelected = Score3; break;
		case 4: ScoreSelected = Score4; break;
		case 5: ScoreSelected = Score5; break;
		case 6: ScoreSelected = Score6; break;
		case 7: ScoreSelected = Score7; break;
	}
}

void PlayerPlay(Player *player)
{
	SelectScore();
    player->currentTick = 0;
    player->lastScoreTick = 0;
    player->decayGenTick = 0;
    player->scorePointer = ScoreSelected;
    PlayUpdateNextScoreTick(player);
    player->status = STATUS_PLAYING;
}

void PlayerInit(Player *player)
{
	SelectScore();
    player->status = STATUS_STOP;
    player->currentTick = 0;
    player->lastScoreTick = 0;
    player->decayGenTick = 0;
    player->scorePointer = ScoreSelected;
    SynthInit(&(player->mainSynthesizer));
}
