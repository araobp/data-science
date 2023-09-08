/**
  ******************************************************************************
  * @file    network_data_params.c
  * @author  AST Embedded Analytics Research Platform
  * @date    Fri Sep  8 03:20:56 2023
  * @brief   AI Tool Automatic Code Generator for Embedded NN computing
  ******************************************************************************
  * Copyright (c) 2023 STMicroelectronics.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file
  * in the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
  ******************************************************************************
  */

#include "network_data_params.h"


/**  Activations Section  ****************************************************/
ai_handle g_network_activations_table[1 + 2] = {
  AI_HANDLE_PTR(AI_MAGIC_MARKER),
  AI_HANDLE_PTR(NULL),
  AI_HANDLE_PTR(AI_MAGIC_MARKER),
};




/**  Weights Section  ********************************************************/
AI_ALIGNED(32)
const ai_u64 s_network_weights_array_u64[498] = {
  0xbd8f3a803d52e4b7U, 0xbdf89c733ea83827U, 0x3e7f66603c9fc7a1U, 0xbb3a957bbef304fdU,
  0x3eb87da4bee37fbbU, 0xbb3b86d1bbb68246U, 0x3f08d9423ec80136U, 0x3ebe3b62bf0702a3U,
  0x3e1e5afebdff81fdU, 0xbe3f5d2a3e83d421U, 0x3ed91cafbeb49be0U, 0x3dcf4ba2be0f9b97U,
  0xbe9931ce3dab87c7U, 0x3e778d7e3de0cd18U, 0xbf3c6a7d3d4966c5U, 0x3d9c220d3ea5da37U,
  0x3e0708df3eb84325U, 0x3e1821513ef10486U, 0xbeb2d0833cf84bb3U, 0x3d945b0a3d712af0U,
  0xbcb542bfbe4f4a86U, 0x3d918bd33e22963eU, 0x3bedd13b3eb0298dU, 0xbe5ca533bdecf698U,
  0xbdcbefefbe8248ffU, 0x3e59a19fbe1da0a1U, 0x3ea7b860be11beccU, 0x3e62e757be3b4c53U,
  0x3d9ff7f83e495486U, 0x3b9082e6be344bc7U, 0xbe05983cbe283f78U, 0xbd77f2413ec8fda8U,
  0xbe8106483e3225a9U, 0xbed7a7c7beec7a82U, 0xbed0be693e26dd76U, 0xbea764913d8a80b6U,
  0x3f4d80d43db9880aU, 0xbed56a4c3e3a49b7U, 0xbe0e12323dfbf0eeU, 0x3d8df570be3d628cU,
  0x3d3ef52b3f192996U, 0x3d9476573e0e8722U, 0x3e0e2210bec7208dU, 0xbe023a86be9724b2U,
  0xbf263e5abec319ecU, 0xbc88427b3ec293bbU, 0xbe351c70be9f00a3U, 0x3edb4bdb3e85ffe0U,
  0x3ef4f8df3e9f50feU, 0x3eec88143f1ee513U, 0xbeb7e773be04763fU, 0x3dfa87463d6e1054U,
  0x3e2474593c9f807eU, 0xbdee351d3d73dacfU, 0xbe2113e8be07d3d1U, 0x3dbad3e9be347b8bU,
  0xbb88f1663dd8c87aU, 0xbcaa5142be5fc443U, 0x3e5f4cc3bef20d82U, 0x3ef64836bd737b37U,
  0x3e44f1ae3d4a4b0fU, 0x3f00a622beb680baU, 0x3e007a5cbe8a2c1aU, 0xbf07db323e1b4088U,
  0xbd0dcb6b3c66f77eU, 0xbd3608cc3e448151U, 0xbe88e9593df59298U, 0x3d4e39a0bedab67eU,
  0xbd7029c3be90ba24U, 0xbe77891b3ddde1a8U, 0x3ea0aff2bd61806dU, 0x3d8d3ebcbe2e148bU,
  0xbeab17983f017cefU, 0xbea4e7bfbe0bf51fU, 0x3d9492dabd9269cfU, 0xbef3386abee75fbdU,
  0x3ed61fd63e507c54U, 0xbe08232d3e917834U, 0xbdecaee63cb767e1U, 0x3ea85e9d3cbda439U,
  0x3ee14fc2bd0e4293U, 0x3e8d08bd3cb7ea01U, 0x3d72143f3efa96eaU, 0xbe88ad233de45836U,
  0x3b228433be09eaf4U, 0x3f0135d33ece9bf0U, 0xbf10c3483c833dcbU, 0x3eb7c0d53c84c370U,
  0xbe3c2ea93e860c02U, 0xbefdf97b3e69bca9U, 0xbe3627233d37b005U, 0xbc52be713eef31eeU,
  0x3e6101433dd05610U, 0x3e38c868ba9526ecU, 0xbe99bf94bd4d8430U, 0x3e49a8f63eb54413U,
  0x3b0048383e6de6ddU, 0xbe7884ecbe9d2337U, 0x3ecdbcea3e464d91U, 0x3e84aed8be2e84a9U,
  0xbe6dc941bdb3895dU, 0x3dfe13c3be77396bU, 0x3e90150cbf058a09U, 0xbcfe20b9beaba373U,
  0x3efd277e3f0890a9U, 0xbe19d9d6bee6fd68U, 0xbe3ff33abe074dedU, 0x3e58600d3dbb6b68U,
  0xbe35e3933e0d801eU, 0x3cfa8bf23dc0d069U, 0xbe02fc003e96f1a0U, 0xbdc357d23de286d6U,
  0x3e76327ebe40f047U, 0xbe26a4c63e2dddbaU, 0xbc455a55be773decU, 0x3e80ed403cec9218U,
  0xbe2903bbbd851b9bU, 0x3dc0cff93e389361U, 0xbe751548bd59a613U, 0xbe8d54e5bca7eaa8U,
  0xbdd379253de69ae2U, 0x3d00d5a1be473f1eU, 0x3ca5f3b1be4af484U, 0xbd7209103ddee722U,
  0x3dd612a83dd8fe48U, 0x3ec8becb3e827e89U, 0x3ee08df73e94157bU, 0x3e4f03b63f11bfadU,
  0xbe0715c83de52232U, 0xbe3ce9e33ed82a79U, 0xbe931d65bea0abd2U, 0x3ea8c9cbbed59577U,
  0xbe12d21c3dcc06e0U, 0xbe8a0a72be45729aU, 0xbcaeaebd3d4e4d00U, 0x3ca76fa0bdb3e8a7U,
  0xbf03f1263e832eb3U, 0x3e763893bc643a46U, 0xbedc3d783da959beU, 0x3f2564ac3eb7d19dU,
  0xbe6439fe3d482a23U, 0xbe20c3e83dee3b82U, 0x3be1f2babdc4207eU, 0xbe13ab043eefe552U,
  0x3ea2b4113e14469fU, 0xbdd394f5bc72d997U, 0x3e52042abea058d9U, 0x3eb25bf2bec4930aU,
  0xbda66cba3e85e9afU, 0x3ef73e773e1fd6b7U, 0x3d8d5be8be7c995fU, 0xbe4120b9be766853U,
  0x3f1e7998bba7875bU, 0x3dcfbffa3f2658f2U, 0x3dc8ab06be10d94bU, 0x3cf9fdcfbd578b9eU,
  0x3ea5bf54bba18202U, 0x3e460734be9f9008U, 0x3f22881d3e1ac8ffU, 0xbd938a023e0ad220U,
  0xbeb5203c3e412a7cU, 0xbd8335923ee32ba0U, 0xbeec18d93d262185U, 0xbed9af493d88e0b3U,
  0xbdf9419cbe905f8eU, 0x3f2876893cc50d6cU, 0xbe61fea23ecee13cU, 0x3f182c693e0753b4U,
  0xbd20b8ee3d6266b0U, 0x3ebe1f7bbed398ceU, 0xbf26ee53be85a579U, 0xbee113e3be2277e4U,
  0x3daaa2313efb34f2U, 0xbf0efd52be29dc77U, 0xbe2067a23cb095d3U, 0x3e962764be080c29U,
  0xbe7d85063dbd2752U, 0xbdffb6b13d56c25bU, 0x3e8de4bd3e92a71aU, 0xbe4cfcdd3ed599c7U,
  0xbebed8c3bd2129d8U, 0xbceff111be6862faU, 0x3e7d710bbe29b457U, 0x3d12f9f4bebb37b6U,
  0xbee81a763e71e371U, 0x3d99dc733ef0a9e8U, 0x3e705c6cbea1116eU, 0x3e880faabe15b634U,
  0x3ea4b5acbeb18c3cU, 0xbe9173643e144408U, 0x3e32b2f2bf1e12c9U, 0x3e6502d4be038213U,
  0x3e88e6cabe70b330U, 0x3e1d4b87bea1297bU, 0xbda8ded1be5e98efU, 0xbe691c983e35b530U,
  0x3e02c89fbdc704f0U, 0x3ec0ef5ebe8816c0U, 0xbf01f8633d245427U, 0xbd0eb594be89244aU,
  0x3d5e23b93e4ab7b8U, 0xbe47d3013d8650aaU, 0xbcdb187bbc9d12e6U, 0xbe234bbebe632af9U,
  0xbc3b13c13efda25bU, 0xbe5c8e14be3b5abfU, 0xbb5465ec3e203c02U, 0x3d4f20e83ea499dbU,
  0xbec0bb05bd8a4ff7U, 0x3bf3bc99be97e0e8U, 0xbf00b1fbbbc9cd75U, 0x3e5b50693ebf3518U,
  0x3e4d39153f1b4781U, 0x3d284d783c85f428U, 0x3e800db1bdf18d82U, 0x3e8b152fbebebcfeU,
  0xbdeb715cbd51619cU, 0x3bb4e96fbe973795U, 0x3e5523dd3e0be050U, 0xbdce152c3d2be7b2U,
  0x3ed2cee43e823ff2U, 0x3f23bdc73f0cd6ebU, 0x3e84d4ef3ed12c5fU, 0x3e895767bf205439U,
  0xbafe1a5cbe36b93aU, 0xbf4fa54b3eb74934U, 0x3ea04730bdf1240eU, 0xbee01a41bebf47fdU,
  0xbdee5ee5bec9811dU, 0x3e1977cfbe7f41d5U, 0x3ce938d9be92d06fU, 0xbe4ce681bbedff3bU,
  0x3e85a9b33e166bf4U, 0x3f0a80703f0f4504U, 0x3e94fc053cd4b7d2U, 0x3e6aafcb3e43b6afU,
  0xbec6c3a03d5e09f8U, 0xbec0f79d3e900f0cU, 0xbec87b6cbf0ba150U, 0x3ed12967bf12bab1U,
  0xbeae99df3e0bddc4U, 0x3d0cbc6fbe5f9ba4U, 0x3e35eb653d24b39aU, 0x3ddee0f53e0da648U,
  0xbc432ad23b905a54U, 0x3c84a7ebbeb895baU, 0xbda497443e168688U, 0xbde17cde3d3b0578U,
  0xbec3b20cbec50431U, 0xbc964e8bbeaaa62fU, 0xbf107221bb870373U, 0xbefba4adbd4c15dbU,
  0x3f01c7c9be32a5fdU, 0x3e967ff23d94587dU, 0x3e48e3a9be5219bbU, 0xbe96a577bea6b484U,
  0x3e033be53e939b74U, 0xbc09040bbdf8944dU, 0x3e18c91a3dd2135cU, 0xbd103902be017c10U,
  0xbd8c718a3e653376U, 0x3ed102a0be0ffe40U, 0x3c7ca1d83e7c942aU, 0x3e9b1dacbe807f30U,
  0x3ef81e7a3e721b4eU, 0xbe0c70d2be6a0058U, 0x3ddab535bea2c6beU, 0x3e8ec27fbdad564eU,
  0x3eb8ffd0be628dbaU, 0xbe1757993eb04dc8U, 0xbe3412dcbe9a1892U, 0xbf05f8423c6a254fU,
  0x3e6d3d5e3dac4c07U, 0xbf01c8e1be10d557U, 0xbd80276d3f129400U, 0x3ddabb683e4e935fU,
  0xbe8d4eec3da8e419U, 0x3e61efeebe771ff9U, 0xbd39880a3e98859aU, 0x3dbe58d63e2e444aU,
  0xbcf051c5bbaa6a61U, 0xbeaccba3be0f028fU, 0x3eac23bebe232e07U, 0xbd8b71663e533086U,
  0xbe4944c83e6ea8d4U, 0xbe0ec6083ee28652U, 0x3eb432403bfdd513U, 0x3ddcd05d3e9294cdU,
  0xbe98dcf8bee8fe9cU, 0xbdc49c34be26cc5eU, 0x3e3f87d3bee10ae2U, 0x3de3171ebea4219cU,
  0xbe51158a3e8e242bU, 0xbe2783cd3f33006cU, 0xbcee2b7fbe012de3U, 0xbd65ca76bd93deadU,
  0x3f092a25be003795U, 0xbd1b27363e888ff1U, 0xbde2f1323e141430U, 0x3d9b06873d6e5278U,
  0xbe216ca7be307af3U, 0xbd93bc48bf1b9f76U, 0x3d61cd8f3ee03e8bU, 0x3db3fec03eaecd50U,
  0x3ee381673ee81e4dU, 0x3e933e8e3ecb17b8U, 0xbd3692573e84db23U, 0x3ee7021abeaca330U,
  0xbe7133ec3f099e2eU, 0x3e9ab93c3de065caU, 0xbd385d69beb65eb4U, 0xbe69e364be068226U,
  0x3c6c16a0be4ee8ebU, 0x3e0c5cbabe123b1bU, 0xbef093f7be68d791U, 0x3e1c747f3f136526U,
  0xbe56583c3ed40fffU, 0xbe690d8dbd3a880bU, 0x3d96f4b03eb1986bU, 0xbe130f07bdb5a371U,
  0xbe7e3c57bf51709eU, 0xbdb16e333e2ca54fU, 0xbd449095be9b4c20U, 0xbec068d53dab6a0bU,
  0x3cef05d0bd2f8bf5U, 0x3d617fb93ec03ea2U, 0xbf0d05aebdaaf3beU, 0x3eb7858b3e1c2046U,
  0xbf1389de3e35427fU, 0xbe50ed693e948673U, 0x3d8ed749bd850685U, 0x3d26daad3e85d67eU,
  0xbe29cb1c3ea6e25fU, 0x3e127630be98651eU, 0x3ef78e73bc652e27U, 0xbc947cbf3e1d30b2U,
  0xbd03718e3e16e51aU, 0xbe815ec4be53f907U, 0x3ea125f4be2dd542U, 0x3e5fe2f4bde72382U,
  0xbe8b4fcdbd7b0440U, 0xbe1a334a3da8c058U, 0x3d23f7c8be901e5aU, 0xbe60b04e3c95d6c0U,
  0x3e8167d83e79f134U, 0xbe00c8b03bac9e00U, 0xbd84a0e8be8fa7cbU, 0x3d6aa0a0be2741b4U,
  0x3e40b2303e922f00U, 0xbe56ff403e37a76aU, 0xbe8ba3a6be74a728U, 0xbecb800a3eaa8cfbU,
  0x3d42bf3d3dcdf46bU, 0xbdd66d013e5e9d63U, 0x3ed27927be2a1fa3U, 0xbef40b7e3e91cd70U,
  0x3e8059103dee6126U, 0x3e39c3f0bd999c8aU, 0x3e889576be225763U, 0xbe9d05773c38c7f2U,
  0x3e86f707bf1727c6U, 0x3e8c6d28bef85eaaU, 0xbd3988f03dfd0565U, 0xbdc7df9abe5b4053U,
  0x3e3c5aefbe388248U, 0x3ddf070fbe35bc20U, 0x3e271b35bebc7a15U, 0x3cec108cbe20210eU,
  0xbdad95453eab39abU, 0x3dd556483ddc9d56U, 0x3dd3cba63db5f4c7U, 0x3e2f11563dcac9dbU,
  0xbe78151ebe0610c0U, 0xbd8712a63e0158e0U, 0xbe10a430be91405aU, 0x3e477106be7253aaU,
  0xbe2a256ebe82cc93U, 0x3e456b383e24b4f8U, 0xbdc106bfbcd91e08U, 0xbcb5f3fe3f01aee1U,
  0xbe87b5d7be6ba36eU, 0xbe607400beb5c1c5U, 0x3d929071be19ee46U, 0xbf314ff13dc6dcf3U,
  0x3dc8ae8abee01545U, 0x3c9312223ec90e96U, 0xbd83ed2a3ec45ed6U, 0xbd2c92f8be818ff1U,
  0xbe9328fdbd55e968U, 0x3dd403b13e3aa36eU, 0x3e6310d7bcd6c5c1U, 0x3d9c0b48bd2a9843U,
  0x3e76328cbe594fb8U, 0x3dc3c6c0bd787700U, 0xbd60fda03e26b00cU, 0xbd9506bc3e7fb31cU,
  0x3d7277483e40d2c0U, 0xbe851e11be608c68U, 0xbe29c1fd3e6c10f8U, 0xbe06f86d3e70a9f4U,
  0xbe044e303d10d798U, 0x3e5d7bc83e3c25d0U, 0xbdb68b3e3e3ed424U, 0x3d438a20be5ecf40U,
  0xbe986410be4eaa0dU, 0xbdc13368be8339e1U, 0x3e849d58bea4a03fU, 0xbdc111debb8bdeacU,
  0x3e591d683c6551a0U, 0xbeb06b5abea8401bU, 0xbc447efdbed743daU, 0x3eacf4bb3dbb097fU,
  0x3e265fc83effd406U, 0x3eb6cca5be103275U, 0x3f1b17b93dd225c8U, 0x3ef9aa253e0acdd1U,
  0x3f019a143eaaa44fU, 0xbe5dae5f3db7679bU, 0x3aadb60f3d902a48U, 0x3e8ebdc93dd1601eU,
  0x3d2b8058be6c86e3U, 0xbe7188ad3cc893b2U, 0x3ee4a2a1be3bbc07U, 0x3e2415ec3ed1d647U,
  0xbdfbb640be818f33U, 0x3c03b9d23df89b3bU, 0x3d7c0ed3be69272eU, 0x3eef1bf9bf0ccf60U,
  0x3de5e8253ea0a6bbU, 0xbdec605b3e3745c1U, 0xbeb0483f3c572aa7U, 0x3e20bdf33e131431U,
  0xbe1a9c1d3e2836eaU, 0x3d7e9f1f3d8e275cU, 0xbdb320733d1d81eeU, 0x3c70154e3cb2acbbU,
  0xbccc48f7bd4ce00cU, 0x3e0d32ec3da7c8ccU, 0xbdeb1afd3c98d00fU, 0xbd7712d1bc2fcf0aU,
  0x3d47b70c3e08f770U, 0x3d0c3780bd669f16U, 0xbdc343853c96a297U, 0xbdeaae103d04e4d4U,
  0xbb81e253U, 0xbd9223cb3d317fd6U, 0xbbc36c45U, 0x3c078b85bcb58043U,
  0x3ec5f149bf18eb4eU, 0xbf57c7d33e48b706U, 0x3eb5cd7fbd836f75U, 0x3de6f6e5bf049774U,
  0xbf4b5dbbbe859808U, 0xbe989e82bf6bd80aU, 0x3f27aa2dbede0119U, 0x3e0356be3ed47413U,
  0xbf036608bf37fdb2U, 0xbf15ec503ee23249U, 0x3f2449b4bf394c39U, 0xbf44124ebf1fbddeU,
  0x3ea3f501bf6cbea0U, 0x3c3b5f06be8c9cefU, 0xbeb2e22dbeef220aU, 0xbf26d1633edcdbfbU,
  0x3e44ad483ee9a3d0U, 0x3ec45453bf323175U, 0xbf470cd33f1b1fe1U, 0xbf4e7da1beee0ad0U,
  0x3f342b8f3e96985aU, 0xbef09b0abea563a3U, 0xbe40965f3f3175e5U, 0xbf827e0b3ed7f9d5U,
  0x3f41160c3ebbf7e3U, 0x3ed2dc93be645bb5U, 0xbdc350d7be2cda84U, 0x3f36dfd63f0ff41bU,
  0x3e7da8663e0795e0U, 0xbead5c373f125cd7U, 0x3b28c1003e68db1cU, 0xbf35d9033ab9233fU,
  0xbf67a96a3e6026e3U, 0x3e7c2a463e57a1acU, 0xbeb2a106bf1ade51U, 0x3e2b93f33ecb5415U,
  0xbdb5eb2c3c67fa2aU, 0x3f1ea79a3f04f5dcU, 0xbe8b605ebdbe1ad3U, 0xbb377038bf8299c7U,
  0xbe686c0b3e610dd3U, 0x3d2a90c53eb0bfaaU, 0x3e0adcd63efa94a8U, 0xbf4f9dfe3ea586adU,
  0xbe8a9bec3d3b2499U, 0x3b959d15bebd7dc8U, 0xbec130aa3f222a5dU, 0x3f31de9ebf46ce9cU,
  0xbdc57e623decfc98U, 0xbc43f434U,
};


ai_handle g_network_weights_table[1 + 2] = {
  AI_HANDLE_PTR(AI_MAGIC_MARKER),
  AI_HANDLE_PTR(s_network_weights_array_u64),
  AI_HANDLE_PTR(AI_MAGIC_MARKER),
};

