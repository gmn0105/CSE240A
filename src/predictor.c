//========================================================//
//  predictor.c                                           //
//  Source file for the Branch Predictor                  //
//                                                        //
//  Implement the various branch predictors below as      //
//  described in the README                               //
//========================================================//
#include <stdio.h>
#include "predictor.h"

//
// TODO:Student Information
//
const char *studentName = "NAME";
const char *studentID   = "PID";
const char *email       = "EMAIL";

//------------------------------------//
//      Predictor Configuration       //
//------------------------------------//

// Handy Global for use in output routines
const char *bpName[4] = { "Static", "Gshare",
                          "Tournament", "Custom" };

int ghistoryBits; // Number of bits used for Global History
int lhistoryBits; // Number of bits used for Local History
int pcIndexBits;  // Number of bits used for PC index
int bpType;       // Branch Prediction Type
int verbose;

//------------------------------------//
//      Predictor Data Structures     //
//------------------------------------//

//
//TODO: Add your own Branch Predictor data structures here
//

uint32_t g_history;
uint32_t gshare_pht_size;
uint32_t *gshare_pht;


//------------------------------------//
//        Predictor Functions         //
//------------------------------------//

// Initialize the predictor
//
void
init_predictor()
{
  //
  //TODO: Initialize Branch Predictor Data Structures
  //
  switch (bpType)
  {
    case STATIC:
      break;
    case GSHARE:
      init_gshare();
      break;
    default:
      break;
  }
}

// Make a prediction for conditional branch instruction at PC 'pc'
// Returning TAKEN indicates a prediction of taken; returning NOTTAKEN
// indicates a prediction of not taken
//
uint8_t
make_prediction(uint32_t pc)
{
  //
  //TODO: Implement prediction scheme
  //

  // Make a prediction based on the bpType
  switch (bpType) {
    case STATIC:
      return TAKEN;
    case GSHARE:
      return make_prediction_gshare(pc);
    case TOURNAMENT:
    case CUSTOM:
    default:
      break;
  }

  // If there is not a compatable bpType then return NOTTAKEN
  return NOTTAKEN;
}

// Train the predictor the last executed branch at PC 'pc' and with
// outcome 'outcome' (true indicates that the branch was taken, false
// indicates that the branch was not taken)
//
void
train_predictor(uint32_t pc, uint8_t outcome)
{
  //
  //TODO: Implement Predictor training
  //
  switch (bpType)
  {
  case STATIC:
    break;
  case GSHARE:
    train_predictor_gshare(pc, outcome);
    break;
  default:
    break;
  }
}

void 
init_gshare() 
{   
    gshare_pht_size = 1 << ghistoryBits;
    gshare_pht = (uint32_t*) malloc(sizeof(uint32_t)*gshare_pht_size);
    g_history = 0;
    memset(gshare_pht, 1, gshare_pht_size*sizeof(uint32_t));
}

uint8_t
make_prediction_gshare(uint32_t pc)
{
    uint32_t history_bits = g_history & (gshare_pht_size - 1);
    uint32_t pc_bits = pc & (gshare_pht_size - 1);
    uint32_t pht_index = history_bits ^ pc_bits;
    
    if (gshare_pht[pht_index] == 0)
    {
        return NOTTAKEN;
    } 
    else if (gshare_pht[pht_index] == 1) 
    {
        return NOTTAKEN;
    }
    else if (gshare_pht[pht_index] == 2) 
    {
        return TAKEN;
    }
    else
    {
      return TAKEN;
    }
}

void 
train_predictor_gshare(uint32_t pc, uint8_t outcome)
{
    uint32_t history_bits = g_history & (gshare_pht_size - 1);
    uint32_t pc_bits = pc & (gshare_pht_size - 1);
    uint32_t pht_index = history_bits ^ pc_bits;

    if (outcome == TAKEN)
    {
        if (gshare_pht[pht_index] < 3)
        {
            gshare_pht[pht_index] = gshare_pht[pht_index] + 1;
        }
    } 
    else
    {
        if (gshare_pht[pht_index] > 0)
        {
            gshare_pht[pht_index] = gshare_pht[pht_index] - 1;
        }
    }

    g_history = (g_history << 1) | outcome;
}