# PORTUGOL Interpreter
##RESUMO
O objetivo deste projeto é criar um interpretador para uma linguagem de programação derivada do Português Estruturado. O objetivo desta linguagem é auxiliar alunos iniciantes de programação no estudo de algoritmos e com o código-fonte do interpretador disponibilizado online possibilitar que qualquer pessoa tenha acesso a ele para melhorá-lo ou utilizá-lo em outros projetos ou estudos.

##Sintaxe da linguagem
*programa* = **algoritmo** *identificador* **⏎** { ( *declarações* | *declaração_função* ) **⏎** } *bloco* **fim_algoritmo**.<br/>
*bloco* =  { ( *decisão* | *declarações* | *retorno* | *se* | *enquanto* | *para* | *faça* | *caso* | *repita* | *até* ) **⏎** }.<br/><br/>
*declarações* =  *declaração_tipo* | *declaração_constante* | **var** *declaração_variável*.<br/>
*decisão* = *identificador* ( *argumentos* | *seletor* **=** *expressão* ).<br/><br/>
*retorno* = **retorne** *expressão*.<br/>
*argumentos* = **(** [ *expressão* { **,** *expressão* } ] **)**.<br/>
*declaração_função* = **funcao** *identificador* [ *parâmetros* ] [ **:** *identificador* ] **⏎** *bloco* **fim_funcao** **⏎**.<br/>
*parâmetros* = **(** *declaração_variável* { **;** *declaração_variável* } **)**.<br/><br/>
*declaração_tipo* = **tipo** *identificador* **=** ( *identificador* | **registro** **⏎** { *declaração_variável* **⏎** } **fim_registro** ).<br/>
*declaração_constante* = **const** *identificador* **=** *expressão*.<br/>
*variável* = *identificador* { **[** *expressão* **]** }.<br/>
*declaração_variável* = *variável* { **,** *variável* } **:** *identificador*.<br/><br/>
*expressão* =  *expressão_simples* { ( **<** | **>** | **<=** | **>=** | **==** ) *expressão_simples*}.<br/>
*expressão_simples* = [ **+** | **-** ] *termo* { ( **+** | **-** | **ou** ) *termo* }.<br/>
*termo* = *fator* { ( __*__ | **/** | **e** ) *fator* }.<br/>
*seletor* = { **.** *identificador* | **[** *expressão* **]** }.<br/>
*fator* = *texto* | *inteiro* | *decimal* | *identificador* ( *seletor* | *argumentos* ) | **(** *expressão* **)** | **nao** *fator*.<br/><br/>
*se* = **se** *expressão* **entao** **⏎** *bloco* { **senao_se** *expressão* **entao** **⏎** *bloco* } [ **senao** **⏎** *bloco* ] **fim_se**.<br/>
*caso* =  **caso** *expressão* **⏎** **seja** *expressão* **:** **⏎** *bloco* { **seja** *expressão* **:** **⏎** *bloco* } [ **senao** **:** **⏎** *bloco* ] **fim_caso**.<br/><br/>
*faça* = **faca** **⏎** *bloco* **sempre_que** *expressão*.<br/>
*enquanto* = **enquanto** *expressão* **faca** **⏎** *bloco* **fim_enquanto**.<br/>
*repita* = **repita** **⏎** *bloco* **ate_que** *expressão*.<br/>
*até* = **ate** *expressão* **repita** **⏎** *bloco* **fim_ate**.<br/>
*para* = **para** *identificador* *seletor* **de** *expressão* **ate** *expressão* [ **passo** *expressão* ] **faca** **⏎** *bloco* **fim_para**.<br/><br/>
