#include "fips202.c"
#include "../timing_and_stat.h"

#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>

#include "../Utils/testReader.c"


uint8_t hexToInt(char hex){
    switch(hex){
        case '0':
            return 0;
        case '1':
            return 1;
        case '2':
            return 2;
        case '3':
            return 3;
        case '4':
            return 4;
        case '5':   
            return 5;
        case '6':
            return 6;
        case '7':
            return 7;
        case '8':
            return 8;
        case '9':
            return 9;
        case 'a':
            return 10;
        case 'b':
            return 11;
        case 'c':
            return 12;
        case 'd':
            return 13;
        case 'e':
            return 14;
        case 'f':
            return 15;
        default:
            return 0;
    }
}

char* byteToHex(uint8_t byte){
    char* hex = malloc(2);
    uint8_t first = byte/16;
    uint8_t second = byte%16;
    switch(first){
        case 0:
            hex[0] = '0';
            break;
        case 1:
            hex[0] = '1';
            break;
        case 2:
            hex[0] = '2';
            break;
        case 3:
            hex[0] = '3';
            break;
        case 4:
            hex[0] = '4';
            break;
        case 5:
            hex[0] = '5';
            break;
        case 6:
            hex[0] = '6';
            break;
        case 7:
            hex[0] = '7';
            break;
        case 8:
            hex[0] = '8';
            break;
        case 9:
            hex[0] = '9';
            break;
        case 10:
            hex[0] = 'a';
            break;
        case 11:
            hex[0] = 'b';
            break;
        case 12:
            hex[0] = 'c';
            break;
        case 13:
            hex[0] = 'd';
            break;
        case 14:
            hex[0] = 'e';
            break;
        case 15:
            hex[0] = 'f';
            break;
    }
    switch(second){
        case 0:
            hex[1] = '0';
            break;
        case 1:
            hex[1] = '1';
            break;
        case 2:
            hex[1] = '2';
            break;
        case 3:
            hex[1] = '3';
            break;
        case 4:
            hex[1] = '4';
            break;
        case 5:
            hex[1] = '5';
            break;
        case 6:
            hex[1] = '6';
            break;
        case 7:
            hex[1] = '7';
            break;
        case 8:
            hex[1] = '8';
            break;
        case 9:
            hex[1] = '9';
            break;
        case 10:
            hex[1] = 'a';
            break;
    }
    //printf("converted %d to %c%c", byte, hex[0], hex[1]);s
    return hex;
}



void testValidator() {
    const char* fileNames[] = {
        "../sha-3bytetestvectors/SHA3_256ShortMsg.rsp",
        "../sha-3bytetestvectors/SHA3_256LongMsg.rsp",
        "../sha-3bytetestvectors/SHA3_384ShortMsg.rsp",
        "../sha-3bytetestvectors/SHA3_384LongMsg.rsp",
        "../sha-3bytetestvectors/SHA3_512ShortMsg.rsp",
        "../sha-3bytetestvectors/SHA3_512LongMsg.rsp"
    };
    for(int fileIndex = 0; fileIndex < 6; fileIndex++){
        const char* fileName = fileNames[fileIndex];
        FILE* file = fopen(fileName, "r");
        if (file == NULL){
            printf("INVALID FILE\n");
            return;
        }
        void (*currentFunction)(uint8_t *, const uint8_t*, size_t);
        int outputLength;
        switch(fileIndex / 2) {
            case 0:
                printf("SHA3-256 ");
                currentFunction = &sha3_256;
                outputLength = 32;
                break;
            case 1:
                printf("SHA3-384 ");
                currentFunction = &sha3_384;
                outputLength = 48;
                break;
            case 2:
                printf("SHA3-512 ");
                currentFunction = &sha3_512;
                outputLength = 64;
                break;
        }

        char inputString[30000];
        int len = 0;
        char expected_output[10000];
        uint8_t output[outputLength];

        int failed = 0;

        while (!feof(file)){

            longmsg(file, inputString, &len, expected_output);

            uint8_t input[len/8];
            for(int i = 0; i < len/8; i++){
                char first = inputString[i*2];
                char second = inputString[i*2+1];
                uint8_t correspondingByte = 0;
                correspondingByte += hexToInt(first)*16;
                correspondingByte += hexToInt(second);
                input[i] = correspondingByte;
            }

            currentFunction(output, input, len/8);
            char converted_output[10000] = {0};

            for(int i = 0; i < outputLength; i++) sprintf(converted_output + 2*i, "%02x", output[i]);

            failed = (strncmp(converted_output, expected_output, outputLength) == 0 ? 0 : 1);
        }
        if (failed) printf("FAILED\n");
        else printf("PASSED\n");
    }
}


int main () {


    size_t len = 32768;
    uint8_t input[len/8];
    uint8_t output512[64], output384[48], output256[32], outputshake256[136], outputshake128[168];
    char* inputString = "27c74d76ffc8ecf7a69970c8584f294b04ee9a485e302bd630821e7ff050c49f9882f10db247adfdb2112c2589e1011f77c48e0f219dbf85e326f8a567324b857735efd60f05edc7b7e21d260fb551c8ac95d02c228f065b62a77912471aff236be62f193f8c151b5b152a131253820f4a6948e78a8e6820550d8b10b79048431d9f981e6a648bc246b13a33b944fdbafa49de8781204d9b636115e5df1d8eab3467142cb613b98421be37cf2d0f2991633b7a562ecf1d9535aafedae848392459478b8c4e2305289445082f963c6d5e2e4a049aba2240d673f03037fa9ab1763445e387581cd978464c959b1b5333e7027b649c4da11e26c43b92443c9a5f696c6c0563fd849c3ae0dec65be4dde2f588d882a40dd51f4dd0940c49d7d0a9c5aac1d96864e5b637090083b61a62e150676846f92545ac124002868df3c4f851954e47e0b6c68f376abcb4f6e5689ac0483399e5bb7a2b3ebc8ee859b6ffb5d6d61a38111ab08f02ab1941616c79740dd34261aef8fa0699eb3f1af54b08461c142d9244b92a1e5f73201240d81cd7feaf9c889d034fa3eb761d05a9d86715ebf8903fc2babca4176ad70fda50da2b5d8549f4fa05006cfc04308fbd86a5880b2a4a25d046ee89f239482179fd39d9f0fc528f0d2596c7943e81a1787c49094351632eb9854935b8887b2e6307c34780bdbe3f1d8c981e7acc172423e3dbff5d15e441c39e541031fe761fe19500ded46f95ee74618ed87755fafe06e2e3d21f20d44538ba9783254443dd3bcf7706b6bbe08358cd015d5381331969a2eae952173b245e009bf45b02ea4fb9deb028ec49a6e612f87815d6fac95b944a77aebea521c57e99e7cc9cdf715ca3ea33aa3fc0efffea097b68c765c4aece0313882a708f10dfac0474b083e2ee401a89f677c9c3b6272892bef06d2df961f545df5f208cedcb6278525f9744ecd99739725c0b2bf3137f467f17b80b249347951c265e214488e3cdd071c3a03db689cb88b52f2e9ef4331e1305ee6616ad228ba545d255fd5f568a55adaefdcb1f17c79f4cdcd59f136fa3e282b846b9f6adb0e38423300098e33848dc01637d5c69b61ee7bb27deb8595b5556beb4f4b8118b3eadf9ba357bb45e13c663db3bb4a8206f4f732c432b19d0d248a7b7af3975a51f86fefc8550ee841d337d6bed71fc8bf94cadecb7b3d88ac2211b58d2c30284ecd9d8fdd65ebc33ceebf71e7bd98c8124a611702099be108ea9c49e469cdfb20f6c2fc512ee44f18eb578f9ce358189582446bf6826f2e99ca84791f10c36b7ee07ac5d1f48ae49c55ba806cccc022cfd8ff5e1759f9da056e64f39bc5d2c19f374f6cce7b423c0dba3304c5ee838f07bafc5df314fe6ba232a829f8fd5eb62847ab61a507acbe03856b8d36dcf4b603b4c5fc0827df6c16a3e88ca53be9b190be0945044e1cd30453ce7a4dfca6201a32e6a8c5270f43d95e80ac2ee5e63c7ef6f3775aa325138681c66c69e21a55d1c1c8f4b887109b40bf1b0904afe6cf398ef489169b681810abfdc41901c3dfb0fe076060cc85db03421213b4ee5de256e286ead6bb2839294eef21e9f035263e240c6c5c6bd17b8783f06cbe15de0e6d9e152cf97717ff36c6f5064b21d0b1eff05288e9e9860553f150649edac9abc41e49c02d53a9e2dfc0a9d1bb0b391b3ccf7436b7ca05f0df169cabc591b35320ef7f34b0d5407c7ab89824b830d0caab3ddc063481e3d6bf604f92c0df2d9cda8e3ffb42708e449e0b2a6fd1273a38c1a80467eea5a21f4b6ae3ca1f079ad17776f69440c9e5a3c054fb239452d7edf6ba97ec54a9c34329a2e3b24ecf8da97e465d903a25e932781264d050482c62e0d1e0f3f502c9dac084e9dbce8b687d5558bf6fad28fb792bc00206b37bca3fe68a8a3f5e55185ea69d40b72cdfcdd5a33ab6930857bed051ea4d272c6213cd9e40edfedef55147526892c4d811204ade78bd9ada1685e090fdc0c2299faba46e91a6d31577e71d4a535a955ed402356d7f4ef7a0f9f3225f76e7684998e44cedf92f5c90615c58f50a02992f9ce63de6dad539eb86890e23e23b79fac2703f72e3f1ebbe361372f8e91550d8e03ebcc1080ac21830aa0c74cc3787cbb0b1f4c3ee99111d5acf03bc6d2d5cd9228e4a82733a30c57cf8c5c89166021af83bf527857f6d3c63c183b622950daef8575fb1c8fdf661efd79295ee318986ed70b934253f4079d5bd6b7c95f6e3d8b62c74b565c0937bfdd91b731f447e9e24f2c9605a333b7424f5238633cd8fd500701233d62c88822b7fc8d6b0f961af1334ae32a105cea9c60b5459887362224bb4c083968c5602fd3e23375fdad3585ca8c03176217a995d82767d00a2fb5f1c8d084b238a7c7ce786a32b5863341855d1b7d36610bfb14fdcf25738b6cbc9e74b41e109817c5e7f0bc119571d147d9341fd9eed5e1e80219d607e9d395421308215fbb51bd63628101587c882e4e6997bfa0a6854078457263959aaa514c38cb3ecd1c2b40a827746190d291f35e1dce2359c83ad1b4c6509f58efd748e4f50734f299ed499d1e110340fe55c77fdd20748cbbbc3ca36dfdb2d74e24231132022a569b617f48309f10f06a9c53ba91d7dac1e7d3284d23c0c39a20578adcd706497f8b4d8bb34368af287d2e3e1621397c81b4473dbbcee0fa2ace39244bcc59b8f7cf7a14e640b209719fd2319c758f83538adf24f457a6800ebe929a69f943a046d1b0c3e710c646523852d453752016c0d1648500a75a7dce5a2a933b460e29f2f7b640c099ec8d54b074ce430a365e4ff19f07e8a54418f309c8b8cb9c007a85ab563279c56b06fd7001c8741a178388525553b338ab7a043236b120e163bc87545641187b8ddad8dec069ef8e0cacf35c5111694ad9cb893f3a2542ff7d167e597f16a7a398316a637b5abb4f5d0119785ae813c214320b979dbd3adc97b1f42499592d24d5323d68f842e04452ab810c3780b887a5d711a226200f62f8b5701c6cb2b3e88c06d85300ca675433d2b382b1826083d4e323c89ff6c977ea497ba9bfc740d605d4c38d5b9a5769479d38409096139de87ec971cee97105b3959c335c43dcd687c5877fd159b86ad73597b09c63dabd2bcf8c057d9f0df39cc25b2209cc8fd05b01ab902aaf923e2bc258389c92bccbe3fff3ec72c0a829edf840df8f0a62accaafb7272d46eecf8b6b04425acac2b05935878e76f5478fee5ed4d0b6a75c521af833aea4c3d3043f5822359cb4f352d59ace5450e6f40de71d9e5e454886ab9303a88c55d14ac58eb23a792bf8579a9e5652a4ca3ab68b1f5c26f10697e69d7ff99e2908165afac2a1d476cf3df670fe909be7aa9996f1df44f5f2bf3c871124019bed873a0b8b214c79944f2bd9bd3b712f86b9ad9a276ffd92c739df6ef0cc44294099e66561f4dcb03b756b07679e2098e7bf1bcc1517ae85da3c27a520bde9cf8a05c27162827802d307a588586fc55e74848e34a41f80579290bc338b3f191633947536771549f6c4ce806576f68a0794cfacd9bace9a8f56fc4720179cfc84a30ea8bc89f377147692a5a5ab7b951dae691dc3406d24b590497074ae1ab9a3423b020c7e6529e4511bbc50de450e282c1b8afa1f444852a73fde38370379fb79e22c2d40b387efbe306c6ff79d1eae75ab9b873d9b2ced03a63a749a9d6312aecdcd27b525babc239b5d08ddcbed39f1e1f77184baf80e0c462b2ebf31a0724ac28e03c703ead3e92238267a17a250088747c0dbe8d53d2ae75a708e0657a3b68a17c85d943ebd798ea5ceb5c8657c2263327c296feb03c5506e41ef66b12b59ed0f7e5e21df0139a64b0a76286043a73f61ae589561e7454a10aaca97d6949ab21e2eb4f2f5279334d3e1a57db830ffe17b5e4fa35f72129de8b107e2ed66e3a3eddf464fa7b8eefee45c2b1098c892112992f6e00f2a94119d618e8f1e279b862499fc801d3bb2ce2781ee292695c999135435969799336dd8bf47e6936129246b64becc8038466a445ad7108a1e0a40e0abf40b1f47587b40d51a2f719bf7456849747df837149ae2efce0cedebbecd39a01b89d0bb69017eaaf0b1e35e6cb0c64f06d9acc18328a946bc6677854e09c5399256d17dd0c83946afb50f31f02f2b7f5c29c55dfbda436987674f7320bc2d8041ba4b15f3981ab241831d9ceb8840df5fb46a94e47a556019549e3d9ead187d11ff660c3c39c9f58c633627c584ac7af5c4c4eda7ce8a3158788b6c2fa62f37e86b49e81272ed177a9d825be7eb1755079ffbf0aa9f87e62e1a3f873a6d1ab6b0481d34dc0c2e21f27828bfb7852b7a7e8e362556b4f7878281e11629cc80024fdc097504d0361adb3d50dc9e1a8df040d99d1513d7801a3bf69aa163880924af703635f183aa0a1f3524380571e8bf37c859474acecaa943c192b1506c5e23b64bccfb0bf035f9a5a5c95d5253e2f049a3924361627e3b812af0cd583f27074eb7f250bade7df055d86ad3ef88238960b16f92c25b44d9dd79dd7ee3c80bd85efad0bb66142af617152d2042d85495633b19bbd381c38a3ff5804b59d0b39fcd5d8fc4775be514d3a33aceb50b1d193a89b846fe9ca4568fe702bc221fc764852857f3557b565171cbba65aee8251fcde373bcaa738e45b5978a59a67dee2d6da34fd1683c24b5d9d5d56b973815ae049172a43ecf2a609a4d41ab4eb2d0046d16a0e51966f748f409b26ad7c394cfc7dade86247820ae24514e39808516f06dc871b7cec07913a8ae7ae9d6514b89fe08dfeac33617373ceada8ba5c068fa5502ebd0f013501dde0e5471fcaae2e491619c5983d1b804cb620d2a26296d280d0c36b7e827246e7f6f6989b19500e8682e3e5bcc10aa1cb7ff0e9d6989ce847e2e79f41dc49e0f711ae1b95ddc4d9e6bb6ec7606bc9588ea0a066cd8a733168eb0dd10f6b265079e57e3896845730d8343cf34999d234e4aca1c20da4d42e526d98992ddb8225eec6e97823a99173d961b0f1cee7bfa78c1897940d641a6f92ff2a50239a07d1d45a6262b3b9fe4378d87d3b66b1ec20d368d4454364c055a8f435be971301d4b9048b80cd4217f0251cf438b794e24169020a6e5613608113014d1ceef31abf0b59c74ae6be593a6c93b882281d2c79ab988c77abfc75624f8ad2f391a6ace159bea83986ab5c62cdc13ca03e97355a1980dce9bb8c7989ed1559eb25aa52fc02c7a0d523757c09b749e30b71af9733415f9e4d0b4fd7584a25646c217a876fc4f9339653c932afb7825e3c57d2ccb59189230a21b3296e053f64ef0b93c4aceec7f2f58df33ac55cff00bb579d3051a8922db98552ea2b9cb7edec5a8295340ec8418b38afdedb06347ce1075b8c97f50ef0c3bcab94218f43620c84d373a5935eb1cffa4bb96141b72575ddba1bd8f5642ec11e39b1e04b5653a810e2ea721b0fc62c395334e89dfb8cc2412578c528162a3a4069bdd85f654547854d541a145fe1387c42ebe18976356ee82a2dcd0ba99587c9c8327a39f4c973688f5b1e0dd3b56d49b738ec82bb91c4193495612ea2d331f0e19a66799932a4210569ee65ef9543081cc65a2840347a8ad16a11ff7675d17c226ffd8ca71362456c0b1bcb2813426deffebac8888fea838cf65ffcb8ffeec2271ddff1b30e365c0fb9269bf1b1f3b1bd5c9f020926acb9c3d4cbb4ced3d2495c4e6e27417588cb8cee8f56f0e3df99e16a7763567b2984128fc5a64c8434982e5c28bbe6dbe21a5035c69a8a4b5e7d08a2c44ad50009790581de4fa6a38e2539d1a02df7b3cacef6095cc5423b08d19f8016472957951ef945862f51943786fb4964dc189fbb6fdae3c265ad45574e22acede6a7e474dc7a555db3f1e8c923ef2dd764ade23c639b4f880a2ffcd2391e63ab87f5392138154bb57bfc13cb8281f988564c4db650a3c72e114ec3f2290f5edfc985b812c836732de5d497b7395026e80f6814b1ad80c515198e2d4fa451f90b29cdf3d1f37a6647901007049ed640871e85e9c6d0cee3ed8162bb4321a2bcbe07527ee7404dbf62f932c44c4980d0d5f22a3f6e60ca7e17f760d065275e345900a7bbab451cc9309fb161e6cfec526538b98800e4102e14da0d1f3e3c00da7c94323cc668842e10d210627c88854fb540d85636c13c6e74b7cbea26c6272989408664a1210058845ee4387609c81336a8fb1c689ebba9a7ff31e3b75c3dac1f8418e5d4151dd31b9481765bca415779dfa65d2eb2fa8a3f3eb37e8864b0dc9fdd6e12c79b392847019c8e96506d96ef634e9af1c4956a9af4d53cf2862d25aabafa8e0459eeb2872479f3de22c92c17c268584a49f8c55b902b818e270f2190bb52aa02a7ab2c6c7bbe486bb7c0b1738b88179099b144f1bf1aeec3ddd36b024ceb195b2afad05785edfef79600b1930d324b8d5a3b53edd017f73c01163c7fee383e664a5a58e8b17d89da33f596f6e5db7668f2136ca051c71d4f3754405a2dde9bcc8c461080bcd16bbc180bec0fa4082aec07c609c9d29ce385e6fa01317a22b3f6775ab1cfd6ae26f5b8d02b4da62cdb6ef1cfb5cac0fdcc68a683e98651c9196c81e49726bd584e1facdedf418ade0a6a469cfb23bb8e4a7fec9e73d953163ea742904b15cf6443b25a84628bc0702a768cba344510b2d0242f863aaaecbe862f1fca481d9b569a26586d7f8cc5a7de1c1aa40bdf6f00100df5f7a86b8d16927c901f18ce9fa3d4041cf660a528d977b3a6e6fc3324fa6c95c64f47abc2b2e60839eb37794ad063d41ebbc095588999d587ff6fe0e1065844171982dc0c17f36a83db1ede2b60dc1ed43a19bde33cb71a5459d18911c865917ed2f48cf7ba4d1bf45b494dcfd9af3ce3ddd68f476741ca8292eb6c459517164408172ee41e458501e8414896bf5fb6b4237199f39ddc9e9916f83adb00ef4b3d1565427fb89f4e42fe916f2e665024cbb7856e7644f9ef1e62a245609040a890a76958191d9cc02e2ab421d573330fa0e68c6d5df10c347912544c74d7cd97052a9de05eb1d09329e913f14a7a8acfca5cf127092686173f829890762be8cef11b6d7b9f19cd2fccc64b6dfbc0a9fdae675e2c165a1a0fe85fd9b1d3212498fb06627a78ca050e9d2ec4e8480a1301d3b22dcce4102a76f9f6b2314a4c038d6958177d7a26faa8019f1b00ddab7884c0b9daba10cd29f4ead39c9f19f0f834e29cf6e4f1c520949093b8381b192ceefbdaff542b05aaf24193034be0d494f7fd417b519e39195cd0a9170e9ef7b2b8fbb2063b713e200774a180d83d5b4c7f0ff23f33dde74759385bd1f4c7bbac7b36cb89c0a1fb1bad2a8b9fb46df98102910b9d1c1f443d224e08537b23d97c9e3383e4943ba1104ba9bdaa711133f55b271a2f7af0f45d30685b261241d5a59a7877c1168ce4806b98b45b8eb59f0bc1488a60c0e16c3a1c3da0c44a8034aa188c1389d83429fe956e0c0d7dd99f26dd6bf8cb9e7f00563e3495a8949d7b0c60d4e3b949bfe5ed51a0214acedbb8e91807d1ea541875342ec3966d70c81d4f3bf974d9fb9eacdf18b5a09de25a70c509a29227824c2f5666812f6d7fbe9fe4e24023566e4ee3466334e66168d45b4d1ad2d61dc998932f6de3bd3301df876ffc6f8bd9024f2da5b9a47f5fb2c7ecd3d40f0a377a6c4541bd71ec58b7a94832f2de2abc681de08f946ae7c360a38a1bbdcdaaff1565ba1fcda2293901ba66f06df26c8af049a0668c1e9c46e2f5c767408534ff3ae3762c26faf07e6071780ab662edcf4b40e1ccf9362954ca4d395904cc34925b83ede941e2de73646cdff474a60af3c9a239256427fb678708346363092662e7b595f9b7004fc27e1b2340111260830bcbfaf2758aaf1999b56d18e3d286eb7ff712bfeec9d0e62dfad660245e7b17724cbbc675d4a0c572e337dc1faee29674d6c8be61b5ee58e48a5de716d0a70bada300141d7c6d05c300193f7f04a5fe76c34c77b83b048542584e57712f90ea1e4d4c6db5088054da9aa5b57a5a5d7de64af27e4aefc005c7d31c13cbb1b53d34ca1535d4ff773d5a1151d0aa3685af53ec7f34a24a8d64d5894bae7f0d806f91a7eeff05d7be19a212a213872f9b0d4016372d46b1e8fd0be951aa13a98f1ba5452e4bc017c194430d1ae0798c2a122b56aedab0dc4f68cb81c27911fc3dabf040778e8c362e17cd7f20ea29f29f58762c6acf69204d22a4d112be029c18ab03184f49c2b9602ea1d75872f0f9873ad115ef7de8045ea51865c6cb5e0fbc934e4b1a002c27e44350a4262d76e76e439ca1a168b61ee07aa69e53339cbd75ef32476c33f0f836e05a642e7c1462b10d693e25024bc69f1dc0195c79372be1396f9bba67e9a0d4a04fa5b5d161e1fbf2a769eefd21a1d7090535272ec5d19aec56b6892e5ec859ed80d760efd7fbab9dd7a3639bc02724c6e69057a6c154ef8278365cd9c8c513329e77c409ff064c598a792770bfcd04c1d4a97283a21c7b965489a9fcc02dbd1a091533c23f985ad03069bf0e6909f3a46feeec47f09eab926c3f0529dd78a3fe412e54ab2228537c59e37ee764747dc908ef496625621bf13fa4d2d3692c5479e7218b174f4cc2c86784fb7e2a830faa4018fe50c8fd395d4f79b77c0bbf6af76bad6bb90f6c253f09acc94533cc35e295fa9fba53c670110c2c07962db844013f106ae2fb1cf76a90e94cdf18966cfc8b0291e54e547cc6f61a67b4579d7c21e1f90d378ab5c5b59dc91aae319821429fdf7974113dabe9bde33c4901d57fd3e4c5b72946ecfcba90aa973527480f6f34dfbae954d889e3940519e6c4f6cb91c8151d3aa82928e40a2c66f6520c63dcfa91daa781cb936f490e74f01987ebd60c6593de36d0b6cc8abcfe745a7beb1641845d51107d54c71d5dc20767ff5d2b1215bfa67b9a42de62eac231991cf195ba25b75899644a95b171a59f48d39c0b79c40505dedc4984432456b0a64603f5b4f47d307516f15585f8842a248b24f1b3eee88ed92d2778abf820740aadbe90d28f137b4c1bc5710b40e23a93ac846850f4ee9412f389060dd4b3ee2eaa7e0183441e8b86d94392d1e944330ebe46f9c1848bd7e4c5dab6c95885a19ad2cec267c20303add608154a502eb60e26c900960c54acf72a99259ee21a10c7ad2d4dc067455629a86f82f52406361a7ca7274efabda5a27840ccdf1fe5ce4d90162e4dc27a0592b7066662c37f35ac264dd83cee4b347f656e070a507d2c852215ce6061f5947188493a13bcde373bca499e104561dbf6487d7ec12254de16e9a8b898a46c854ea2c468788eb7ad61bf16d3499eb62eafb7fb9e6403e1356b4d4b776728235a628cf49c01a7a01a86ab222259689c9ef42a74e09383b6d50ad38a855c6bde7685ad462f5fa60c0142e51805b021a99f6c1655dfa0105a3f7df2c25cd8cde27c009a55ace2e87ff3ff1354f7fe5851b290122b064a382fd72d419f886cecd3dd6bca9f8f2c0d506543ac6573848b297ae89470ded279cc3ea1468e729937271d3fb48060ab7341c79926c602f328954ade13a3d3d943a3c86257fbb5ee4b50adf4eea4abd1c8b8bd6808f310babf29c4f926508fdf244b16f91c83bccc87e52213ef78eb4f6c199958c979375bbd5b5bb232f35d549a16acce0a9311c47b58fc252d798964dc08fdb3eec4e156b1ae81a1cff8e6ca23bafc6988c63f569420ae913f3300c2a6fb5d31ff62a92c2a97d94063fd96b04f9c88543d89b00370d0bbf07b120be94b652a2b61eef95c86abc506856df7978c3fc3068f59fe5b8feda6cb87a2f53937daf738bca64c58646ed77caac683f058195f0904b374bf4febe17aeb724742fac156c276352cb03235730b4a93c65b31bd9ec42422ddaf4f301e96fcec8c8712ccab51152814e48eb43a4afb522928a7a114d0642483bbfc7e9098529e3f860e31677d1feb9f75b84f9fb4238e36e9384843a64b34f165d60bf9f782e3dab04ba43daf0cec26e46d8c15cf69a47a2d5227cdce5fd0b12d7a8cad5ce479d8a66999805f52c635a11cde3ef524316d6583d3f5a108844d8348554d111dbcc3c8d695c21687f6663c24da6dede9b18b125bb16dd6ebcc1107ce1a3bbc851936a8110d855d22e9132633f544e220b15dc4386498d85024c61b8a300bc7c13b8bc4b7854cabaa3ad6ffb8a3369a7f9d4ffba842091e0c0ab73efb3b3fcb48803d9f28717a7a84581c293188c57f4ce1ec1939fe312045fa7ef29f904a2f8183e6a7e276b15247cd7d132d0a64091f3bbcad5bdd9377b48087d6e5c3bd6d02b2f16f83f963cb7b07547e09acb4b07ce73c388c84b29cce296c4c7c79fc2c529a08667b7e143e84924caa55e41a0ddc90e54b5a781";
    for(int i = 0; i < len/8; i++){
        char first = inputString[i*2];
        char second = inputString[i*2+1];
        uint8_t correspondingByte = 0;
        correspondingByte += hexToInt(first)*16;
        correspondingByte += hexToInt(second);
        input[i] = correspondingByte;
    }
    welford_t welford512, welford384, welford256, welfordshake128, welfordshake256;
    welford_init(&welford512);
    welford_init(&welford384);
    welford_init(&welford256);
    welford_init(&welfordshake128);
    welford_init(&welfordshake256);
    for (int j = 0; j < 100000; j++){
        uint64_t start = getTime();
        sha3_512(output512, input, len/8);
        uint64_t end = getTime();
        welford_update(&welford512,(long double) (end - start)/(len/8));
    }
    for (int j = 0; j < 100000; j++){
        uint64_t start = getTime();
        sha3_384(output384, input, len/8);
        uint64_t end = getTime();
        welford_update(&welford384,(long double) (end - start)/(len/8));
    }
    for (int j = 0; j < 100000; j++){
        uint64_t start = getTime();
        sha3_256(output256, input, len/8);
        uint64_t end = getTime();
        welford_update(&welford256,(long double) (end - start)/(len/8));
    }
    for (int j = 0; j < 100000; j++){
        uint64_t start = getTime();
        shake256(outputshake256, 136, input, len/8);
        uint64_t end = getTime();
        welford_update(&welfordshake256,(long double) (end - start)/(len/8));
    }
    for (int j = 0; j < 100000; j++){
        uint64_t start = getTime();
        shake128(outputshake128, 168, input, len/8);
        uint64_t end = getTime();
        welford_update(&welfordshake128,(long double) (end - start)/(len/8));
    }
    printf("SHA3-512: ");
    welford_print(welford512);
    printf("\n");
    for(int i = 0; i < 64; i++){
        printf("%x", output512[i]);
    }
    printf("\n");
    printf("SHA3-384: ");
    welford_print(welford384);
    printf("\n");
    for(int i = 0; i < 48; i++){
        printf("%x", output384[i]);
    }
    printf("\n");
    printf("SHA3-256: ");
    welford_print(welford256);
    printf("\n");
    for(int i = 0; i < 32; i++){
        printf("%x", output256[i]);
    }
    printf("\n");
    printf("SHAKE-256 (OUTPUT: 136B): ");
    welford_print(welfordshake256);
    printf("\n");
    for(int i = 0; i < 136; i++){
        printf("%x", outputshake256[i]);
    }
    printf("\n");
    printf("SHAKE-128 (OUTPUT: 168B): ");
    welford_print(welfordshake128);
    printf("\n");
    for(int i = 0; i < 168; i++){
        printf("%x", outputshake128[i]);
    }
    printf("\n");
    //testValidator();
    return 0;
}