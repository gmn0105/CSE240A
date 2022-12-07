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

uint32_t global_history;
uint32_t local_history;

uint32_t gshare_pht_size;
uint32_t *gshare_pht;


uint32_t tournament_local_bht_size;
uint32_t tournament_local_pht_size;
uint32_t tournament_global_pht_size;
uint32_t tournament_choice_pht_size;

uint32_t *tournament_local_bht;
uint32_t *tournament_local_pht;
uint32_t *tournament_global_pht;
uint32_t *tournament_choice_pht;


//------------------------------------//
//        Predictor Functions         //
//------------------------------------//

// Initialize the predictor
//

void 
init_gshare() 
{   
    gshare_pht_size = 1 << ghistoryBits;
    gshare_pht = (uint32_t*) malloc(sizeof(uint32_t)*gshare_pht_size);
    global_history = 0;
    for (int i = 0; i < gshare_pht_size; i++)
    {
      gshare_pht[i] = 1;
    }
}

uint8_t
make_prediction_gshare(uint32_t pc)
{
    uint32_t history_bits = global_history & (gshare_pht_size - 1);
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
    uint32_t history_bits = global_history & (gshare_pht_size - 1);
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

    global_history = (global_history << 1) | outcome;
}


void
init_tournament()
{
  //Local History
    // Local Branch History Table

  tournament_local_bht_size = 1 << pcIndexBits;
  tournament_local_bht = (uint32_t*) malloc(sizeof(uint32_t)*tournament_local_bht_size);
  for (int i = 0; i < tournament_local_bht_size; i++)
  {
    tournament_local_bht[i] = 0;
  } 

    // Local Pattern History Table

  tournament_local_pht_size = 1 << lhistoryBits;
  tournament_local_pht = (uint32_t*) malloc(sizeof(uint32_t)*tournament_local_pht_size);
  for (int i = 0; i < tournament_local_pht_size; i++)
  {
    tournament_local_pht[i] = 1;
  } 
  // Global History

  tournament_global_pht_size = 1 << ghistoryBits;
  tournament_global_pht = (uint32_t*) malloc(sizeof(uint32_t)*tournament_global_pht_size);
  for (int i = 0; i < tournament_global_pht_size; i++)
  {
    tournament_global_pht[i] = 1;
  } 
  // Choice History
  tournament_choice_pht_size = 1 << ghistoryBits;
  tournament_choice_pht = (uint32_t*) malloc(sizeof(uint32_t)*tournament_choice_pht_size);
  for (int i = 0; i < tournament_choice_pht_size; i++)
  {
    tournament_choice_pht[i] = 2;
  } 
  global_history = 0;
}

uint8_t make_prediction_tournament(uint32_t pc)
{
  uint32_t global_history_bits = global_history & (tournament_global_pht_size - 1);
  uint32_t tournament_choice = tournament_choice_pht[global_history_bits];
  uint32_t pc_index = pc & (tournament_local_bht_size-1);
  uint32_t local_history_bits = tournament_local_bht[pc_index] & (tournament_local_pht_size-1);
  uint32_t tournament_prediction;

  if(tournament_choice > 1)
  {
    tournament_prediction = tournament_global_pht[global_history_bits];
  }
  else
  {
    tournament_prediction = tournament_local_pht[local_history_bits];
  }

  if (tournament_prediction < 2)
  {
    return NOTTAKEN;
  }
  else
  {
    return TAKEN;
  }

}

void
train_predictor_tournament(uint32_t pc, uint8_t outcome)
{
  uint32_t global_history_bits = global_history & (tournament_global_pht_size - 1);
  uint32_t tournament_choice = tournament_choice_pht[global_history_bits];
  uint32_t pc_index = pc & (tournament_local_bht_size-1);
  uint32_t local_history_bits = tournament_local_bht[pc_index] & (tournament_local_pht_size-1);
  uint32_t tournament_local_prediction = tournament_local_pht[local_history_bits];
  uint32_t tournament_global_prediction = tournament_global_pht[global_history_bits];

  if(tournament_local_prediction>1)
    tournament_local_prediction = TAKEN;
  else
    tournament_local_prediction = NOTTAKEN;

  if(tournament_global_prediction>1)
    tournament_global_prediction = TAKEN;
  else
    tournament_global_prediction = NOTTAKEN;

  if(tournament_global_prediction == outcome && tournament_local_prediction != outcome && tournament_choice < 3)
  {
    tournament_choice_pht[global_history_bits] = tournament_choice_pht[global_history_bits] + 1;
  }
  else if(tournament_global_prediction != outcome && tournament_local_prediction == outcome && tournament_choice > 0)
  {
    tournament_choice_pht[global_history_bits] = tournament_choice_pht[global_history_bits] - 1;
  }

  if(outcome == TAKEN)
  {
    if(tournament_global_pht[global_history_bits] < 3)
      tournament_global_pht[global_history_bits] = tournament_global_pht[global_history_bits] + 1;
    if(tournament_local_pht[local_history_bits] < 3)
      tournament_local_pht[local_history_bits] = tournament_local_pht[local_history_bits] + 1;
  }
  else
  {
    if(tournament_global_pht[global_history_bits] > 0)
      tournament_global_pht[global_history_bits] = tournament_global_pht[global_history_bits] - 1;
    if(tournament_local_pht[local_history_bits] > 0)
      tournament_local_pht[local_history_bits] = tournament_local_pht[local_history_bits] - 1;
  }

  tournament_local_bht[pc_index] = ((tournament_local_bht[pc_index]<<1) | outcome) & (tournament_local_pht_size-1);
  global_history = ((global_history << 1) | outcome) & (tournament_global_pht_size-1);

}


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
    case TOURNAMENT:
      init_tournament();
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
      return make_prediction_tournament(pc);
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
  case TOURNAMENT:
    train_predictor_tournament(pc, outcome);
    break;
  default:
    break;
  }
}