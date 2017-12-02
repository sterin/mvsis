/**CFile****************************************************************

  FileName    [autiDot.c]

  PackageName [MVSIS 1.3: Multi-valued logic synthesis system.]

  Synopsis    [Outputs of the STG of automata into DOT files to be 
  interpreted by DOT software from ATT reseach lab. 
  This software creates PostScript, which can be viewed or printed. 
  http://www.research.att.com/sw/tools/graphviz/ ]

  Author      [MVSIS Group]
  
  Affiliation [UC Berkeley]

  Date        [Ver. 1.0. Started - February 1, 2003.]

  Revision    [$Id: autiDot.c,v 1.1 2004/02/19 03:06:51 alanmi Exp $]

***********************************************************************/

#include "autiInt.h"

////////////////////////////////////////////////////////////////////////
///                        DECLARATIONS                              ///
////////////////////////////////////////////////////////////////////////

static void Auti_AutoDotWriteBlifCover( FILE * pFile, Mvc_Cover_t * pMvc );
static void Auti_AutoDotWriteBlifMvCover( FILE * pFile, Aut_Auto_t * pAut, 
    Vm_VarMap_t * pVm, Aut_Var_t ** pVars, Mvc_Cover_t * Cover );


////////////////////////////////////////////////////////////////////////
///                     FUNCTION DEFITIONS                           ///
////////////////////////////////////////////////////////////////////////

/**Function*************************************************************

  Synopsis    [Writes the STG of the automaton into the DOT file.]

  Description []
               
  SideEffects []

  SeeAlso     []

***********************************************************************/
void Auti_AutoWriteDot( Aut_Auto_t * pAut, char * pFileName, int nStatesMax, int fShowCond )
{
    Aut_State_t * pState;
    Aut_Trans_t * pTrans;
    FILE * pFile;
    int nLevels, Level, s;
    char Buffer1[20], Buffer2[20];
    int nStatesIncomp, nStatesNonDet;
    Mvc_Manager_t * pMvcMan;

    if ( pAut->nStates > nStatesMax )
    {
        printf( "The automaton has more than %d states. DOT file is not written.\n", nStatesMax );
        return;
    }
    
    // create ZDD variables for writing the covers
    Cudd_zddVarsFromBddVars( Aut_AutoReadDd(pAut), 2 );

    // perform the reachability analysis and set the level of the states
    nLevels = Aut_AutoReachability( pAut );

    // start the stream
	pFile = fopen( pFileName, "w" );

	// write the DOT header
	fprintf( pFile, "# %s\n",  "Automaton generated by Aut_AutoWriteDot() procedure in MVSIS 2.0." );
	fprintf( pFile, "\n" );
	fprintf( pFile, "digraph Automaton {\n" );
	fprintf( pFile, "size = \"7.5,10\";\n" );
//	fprintf( pFile, "ranksep = 0.5;\n" );
//	fprintf( pFile, "nodesep = 0.5;\n" );
	fprintf( pFile, "center = true;\n" );
    fprintf( pFile, "edge [fontsize = 10];\n" );
//	fprintf( pFile, "edge [dir = none];\n" );
	fprintf( pFile, "\n" );

	// labels on the left of the picture
	fprintf( pFile, "{\n" );
	fprintf( pFile, "  node [shape = plaintext];\n" );
	fprintf( pFile, "  edge [style = invis];\n" );
	fprintf( pFile, "  LevelTitle1 [label=\"\"];\n" );
	fprintf( pFile, "  LevelTitle2 [label=\"\"];\n" );
	// genetate node names with labels
	for ( Level = 0; Level <= nLevels; Level++ )
	{
		// the visible node name
		fprintf( pFile, "  Level%d", Level );
		fprintf( pFile, " [label = " );
		// label name
		fprintf( pFile, "\"" );
//        if ( Level != nLevels )
//		    fprintf( pFile, "%d", Level+1 );
		fprintf( pFile, "\"" );
		fprintf( pFile, "];\n" );
	}

	fprintf( pFile, "  LevelTitle1 ->  LevelTitle2 ->" );
	// genetate the sequence of visible/invisible nodes to mark levels
	for ( Level = 0; Level <= nLevels; Level++ )
	{
		// the visible node name
		fprintf( pFile, "  Level%d",  Level );
		// the connector
		if ( Level != nLevels )
			fprintf( pFile, " ->" );
		else
			fprintf( pFile, ";" );
	}
	fprintf( pFile, "\n" );
	fprintf( pFile, "}" );
	fprintf( pFile, "\n" );
	fprintf( pFile, "\n" );

	// generate title box on top
	fprintf( pFile, "{\n" );
	fprintf( pFile, "  rank = same;\n" );
	fprintf( pFile, "  LevelTitle1;\n" );
	fprintf( pFile, "  title1 [shape=plaintext,\n" );
	fprintf( pFile, "          fontsize=20,\n" );
	fprintf( pFile, "          fontname = \"Times-Roman\",\n" );
	fprintf( pFile, "          label=\"" );
	fprintf( pFile, "%s", "Automaton generated by MVSIS 2.0." );
	fprintf( pFile, "\\n" );
	fprintf( pFile, "Benchmark \\\"%s\\\". ", pAut->pName );
	fprintf( pFile, "Time was %s. ",  Extra_TimeStamp() );
	fprintf( pFile, "\"\n" );
	fprintf( pFile, "         ];\n" );
	fprintf( pFile, "}" );
	fprintf( pFile, "\n" );
	fprintf( pFile, "\n" );

	// generate statistics box
	fprintf( pFile, "{\n" );
	fprintf( pFile, "  rank = same;\n" );
	fprintf( pFile, "  LevelTitle2;\n" );
	fprintf( pFile, "  title2 [shape=plaintext,\n" );
	fprintf( pFile, "          fontsize=18,\n" );
	fprintf( pFile, "          fontname = \"Times-Roman\",\n" );
	fprintf( pFile, "          label=\"" );
//	fprintf( pFile, "Inputs = %d. ",      pAut->nVars );
//  fprintf( pFile, "Outputs = %d. ",     pAut->nOutputs );

//	fprintf( pFile, "The automaton is %s and %s.",  
//        (Aut_AutoComplete( pAut, pAut->nVars, 0, 1 )? "incomplete": "complete"),
//        (Aut_AutoCheckNd( stdout, pAut, pAut->nVars, 0 )? "non-deterministic": "deterministic") );

    nStatesIncomp = Auti_AutoCheckIsComplete( pAut, pAut->nVars );
    nStatesNonDet = Auti_AutoCheckIsNd( stdout, pAut, pAut->nVars, 0 );
    sprintf( Buffer1, " (%d states)", nStatesIncomp );
    sprintf( Buffer2, " (%d states)", nStatesNonDet );

	fprintf( pFile, "The automaton is %s%s and %s%s.", 
        (nStatesIncomp? "incomplete" : "complete"),
        (nStatesIncomp? Buffer1 : ""),
        (nStatesNonDet? "non-deterministic": "deterministic"),
        (nStatesNonDet? Buffer2 : "")     );

    fprintf( pFile, "\\n" );

    fprintf( pFile, "%d inputs  ",       pAut->nVars );
	fprintf( pFile, "%d states  ",       pAut->nStates );
	fprintf( pFile, "%d transitions",    Aut_AutoCountTransitions(pAut) );
//	fprintf( pFile, "%d products",       Aut_AutoCountProducts(pAut) );
	fprintf( pFile, "\\n" );

    fprintf( pFile, "Inputs = {" );
    for ( s = 0; s < pAut->nVars; s++ )
    {
        fprintf( pFile, "%s %s", ((s==0)? "": ","),  pAut->pVars[s]->pName );
        if ( pAut->pVars[s]->nValues > 2 )
            fprintf( pFile, "(%d)", pAut->pVars[s]->nValues );
    }
    fprintf( pFile, " }" );
	fprintf( pFile, "\\n" );
/*
    // print the latch values
    for ( s = 0; s < pAut->nVarsL; s++ )
    {
        if ( pAut->pVarsL[s]->pValueNames == NULL )
            continue;
        fprintf( pFile, "Latch%d = {", s+1 );
        for ( s = 0; s < pAut->pVarsL[s]->nValues; s++ )
        {
            fprintf( pFile, " %s", pAut->pVarsL[s]->pValueNames[s] );
            if ( pAut->pVarsL[s]->nValues > 2 )
                fprintf( pFile, "(%d)", pAut->pVarsL[s]->nValues );
        }
        fprintf( pFile, " }" );
	    fprintf( pFile, "\\n" );
    }
*/
	fprintf( pFile, "\"\n" );
	fprintf( pFile, "         ];\n" );
	fprintf( pFile, "}" );
	fprintf( pFile, "\n" );
	fprintf( pFile, "\n" );
/*
	// generate the square node on top
	fprintf( pFile, "{\n" );
	fprintf( pFile, "  rank = same;\n" );
	fprintf( pFile, "  node [shape=polygon, sides=7, peripheries=3];\n" );
	fprintf( pFile, "  edge [style = invis];\n" );
	for ( out = 0; out < pNet->nRoots; out++ )
		fprintf( pFile, "  title3_%d [label=\"%s\"];\n", out, pOutputs[out] );
	fprintf( pFile, "}" );
	fprintf( pFile, "\n" );
	fprintf( pFile, "\n" );
*/
/*
	// generate nodes of each rank
	for ( Level = 0; Level < nLevels; Level++ )
	{
	    fprintf( pFile, "{\n" );
	    fprintf( pFile, "  rank = same;\n" );
//		fprintf( pFile, "  node [shape=record,width=1,height=0.75];\n" );
		fprintf( pFile, "  node [shape = circle];\n" );
		// the labeling node of this level
		fprintf( pFile, "  Level%d;\n",  Level );

//        for ( s = 0; s < pAut->nStates; s++ )
//            if ( pAut->pStates[s]->Level == Level && !pAut->pStates[s]->fRankLess )
//		        fprintf( pFile, "  Node%d [label = \"%s\"];\n", s, pAut->pStates[s]->pName );

		fprintf( pFile, "}" );
		fprintf( pFile, "\n" );
		fprintf( pFile, "\n" );
	}

    // put the unreachable states
    {
	    fprintf( pFile, "{\n" );
	    fprintf( pFile, "  rank = same;\n" );
//		fprintf( pFile, "  node [shape=record,width=1,height=0.75];\n" );
		fprintf( pFile, "  node [shape = circle];\n" );
		// the labeling node of this level
		fprintf( pFile, "  Level%d;\n",  nLevels );

//        for ( s = 0; s < pAut->nStates; s++ )
//            if ( pAut->pStates[s]->Level == UNREACHABLE && !pAut->pStates[s]->fRankLess )
//		        fprintf( pFile, "  Node%d [label = \"%s\"];\n", s, pAut->pStates[s]->pName );

		fprintf( pFile, "}" );
		fprintf( pFile, "\n" );
		fprintf( pFile, "\n" );
    }
*/

    {
		fprintf( pFile, "{\n" );
   	    fprintf( pFile, "  node [fixedsize = true];\n" );
	    fprintf( pFile, "  node [fontsize = 14];\n" );
//		fprintf( pFile, "  node [shape = circle];\n" );
		fprintf( pFile, "  node [hight = 0.7];\n" );
		fprintf( pFile, "  node [width = 1.0];\n" );
//		fprintf( pFile, "  node [style = bold];\n" );

        // print the nodes
        s = 0;
//        for ( s = 0; s < pAut->nStates; s++ )
        Aut_AutoForEachState( pAut, pState )
        {
	        fprintf( pFile, "  Node%d [style=filled, label = \"%s\"", s, pState->pName );
            fprintf( pFile, "%s", (s == 0? ", shape = octagon": "") );
//            fprintf( pFile, "%s", (pAut->pStates[s]->fAcc? ", style = bold": "") );
            fprintf( pFile, "%s", (pState->fAcc? ", color = \".3 .7 1.0\", fillcolor = \".3 .7 1.0\"": ", color = coral, fillcolor = coral") );
	        fprintf( pFile, "];\n" );
            pState->uData = s;
            s++;
        }

		fprintf( pFile, "}" );
		fprintf( pFile, "\n" );
		fprintf( pFile, "\n" );
    }
	
	// generate invisible edges from the square down
	fprintf( pFile, "title1 -> title2 [style = invis];\n" );
    fprintf( pFile, "title2 -> Node0 [style = invis];\n" );

	// generate edges
    pMvcMan = Mvc_ManagerStart();
    Aut_AutoForEachState( pAut, pState )
    {
        Aut_StateForEachTransitionFrom( pState, pTrans )
        {
    		// generate the edge from this node to the next (or input variable)
			fprintf( pFile, "Node%d",  pTrans->pFrom->uData );
			fprintf( pFile, " -> " );
			fprintf( pFile, "Node%d",  pTrans->pTo->uData );
            if ( fShowCond )
            {
                Vm_VarMap_t * pVm = Vmx_VarMapReadVm(pAut->pVmx);
                Mvc_Cover_t * pMvc;
                DdNode * bFunc;

                // get the function
                bFunc = Aut_TransReadCond(pTrans);
                // derive the cover for the condition
                pMvc = Fnc_FunctionDeriveSopFromMddSpecial( pMvcMan, 
                        Aut_AutoReadDd(pAut), bFunc, bFunc, pAut->pVmx, pAut->nVars );
                // write the cover
			    fprintf( pFile, " [label = \"" );
//                Auti_AutoDotWriteBlifCover( pFile, pTrans->pCond );
                Auti_AutoDotWriteBlifMvCover( pFile, pAut, pVm, pAut->pVars, pMvc );
			    fprintf( pFile, "\"]" );
            }
			fprintf( pFile, ";\n" );
        }
    }
    Mvc_ManagerFree(pMvcMan);

	fprintf( pFile, "}" );
	fprintf( pFile, "\n" );
	fprintf( pFile, "\n" );
	fclose( pFile );
}

/**Function*************************************************************

  Synopsis    [Performs reachability analysis.]

  Description []
               
  SideEffects []

  SeeAlso     []

***********************************************************************/
void Auti_AutoDotWriteBlifCover( FILE * pFile, Mvc_Cover_t * pMvc )
{
    Mvc_Cube_t * pCube;
    int Value0, Value1, v;
    int fFirst;

    fFirst = 1;
    Mvc_CoverForEachCube( pMvc, pCube )
    {
        if ( fFirst )
            fFirst = 0;
        else
            fprintf( pFile, "\\n" );
        for ( v = 0; v < pMvc->nBits/2; v++ )
        {
            Value0 = Mvc_CubeBitValue( pCube, 2*v );
            Value1 = Mvc_CubeBitValue( pCube, 2*v + 1 );
            if ( Value0 && Value1 )
                fprintf( pFile, "-" );
            else if ( Value0 && !Value1 )
                fprintf( pFile, "0" );
            else if ( !Value0 && Value1 )
                fprintf( pFile, "1" );
            else
            {
                assert( 0 );
            }
        }
    }
}

/**Function*************************************************************

  Synopsis    [Write the cover as BLIF-MV.]

  Description []
               
  SideEffects []

  SeeAlso     []

***********************************************************************/
void Auti_AutoDotWriteBlifMvCover( FILE * pFile, Aut_Auto_t * pAut, 
    Vm_VarMap_t * pVm, Aut_Var_t ** pVars, Mvc_Cover_t * Cover )
{
    Mvc_Cube_t * pCube;
    char ** pValueNames;
    int Counter, FirstValue;
    int i, v;
    int fFirst;

    int * pValues      = Vm_VarMapReadValuesArray(pVm);
    int * pValuesFirst = Vm_VarMapReadValuesFirstArray(pVm);
//    int nVarsIn        = Vm_VarMapReadVarsInNum(pVm);
    int nVarsIn        = Aut_AutoReadVarNum(pAut);

    fFirst = 1;
    Mvc_CoverForEachCube( Cover, pCube ) 
    {
        if ( fFirst )
            fFirst = 0;
        else
            fprintf( pFile, "\\n" );
        // write the variables
        for ( i = 0; i < nVarsIn; i++ )
        {
            pValueNames = Aut_VarReadValueNames( pVars[i] );

            // count the number of values in this literal
            Counter  = 0;
            for ( v = 0; v < pValues[i]; v++ )
                if ( Mvc_CubeBitValue( pCube,  pValuesFirst[i] + v) ) 
                    Counter++;
            assert( Counter > 0 );

            if ( Counter == pValues[i] )
                fprintf( pFile, "- " );
            else if ( Counter == 1 )
            {
                for ( v = 0; v < pValues[i]; v++ )
                    if ( Mvc_CubeBitValue( pCube,  pValuesFirst[i] + v) ) 
                    {
                        if ( pValueNames )
                            fprintf( pFile, "%s ", pValueNames[v] );
                        else
                            fprintf( pFile, "%d ", v );
                    }
            }
            else
            {
                fprintf( pFile, "(" );
                FirstValue = 1;
                for ( v = 0; v < pValues[i]; v++ )
                    if ( Mvc_CubeBitValue( pCube,  pValuesFirst[i] + v) ) 
                    {
                        if ( FirstValue )
                            FirstValue = 0;
                        else
                            fprintf( pFile, "," );
                        if ( pValueNames )
                            fprintf( pFile, "%s", pValueNames[v] );
                        else
                            fprintf( pFile, "%d", v );
                    }
                fprintf( pFile, ") " );
            }
        }
        // write the output
//        if ( nVarsIn > 0 )
//            fprintf( pFile, " " );
//        fprintf( pFile, " %*s  %*s\n", nLength, pName1, nLength, pName2 );
    }
}

////////////////////////////////////////////////////////////////////////
///                       END OF FILE                                ///
////////////////////////////////////////////////////////////////////////


