
1 create folder "first_contract"
2 run -> "yarn init" (press enter many times)
3 run -> "yarn add typescript ts-node @types/node @swc/core --dev"

4 add file "tsconfig.json"
```text
{
    "compilerOptions": {
        "target": "es2020",
        "module": "CommonJS",
        "esModuleInterop": true,
        "forceConsistentCasingInFileNames": true,
        "strict": true,
        "skipLibCheck": true,
        "resolveJsonModule": true
    },
    "ts-node": {
        "transpileOnly": true,
        "transpiler": "ts-node/transpilers/swc"
    }
}
```

5 run -> "yarn add ton-core ton-crypto @ton-community/func-js --dev"

6 create folder "contracts"
7 create file "contracts/main.fc"
```
() recv_internal(int my_balance, int msg_value, cell in_msg_full, slice in_msg_body) impure {
    
}
```

8 create folder "scripts"
9 create file "scripts/compile.ts"
```
import * as fs from "fs";
import process from "process";
import { Cell } from "ton-core";
import { compileFunc } from "@ton-community/func-js";

async function compileScript() {
    
    console.log(
        "======================================================"
    );
    console.log(
        "Compile script is running. Let's find FunC code to compile..."
    );

    const compileResult = await compileFunc ({
        targets: ["./contracts/main.fc"],
        sources: (x) => fs.readFileSync(x).toString("utf8"),
    });

    if (compileResult.status === "error") {
        console.log(" - OH NO! Compilation errors! The compiler output was:");
        console.log(`\n${compileResult.message}`);
        process.exit(1);
    }

    console.log(" - Compilation successful!");

    const hexArtifact = `build/main.compiled.json`;

    fs.writeFileSync(
        hexArtifact,
        JSON.stringify({
            hex: Cell.fromBoc(Buffer.from(compileResult.codeBoc, "base64"))[0]
              .toBoc()
              .toString("hex"),
        })
    );

    console.log(" - Compiled code saved to " + hexArtifact);
}

compileScript()
```

10 create folder "build"
11 edit file "package.json"
```
{
    ...
    ...
    ...,
    "scripts": {
        "compile": "ts-node ./scripts/compile"
    }
}
```

11 run -> "yarn compile"


--------------------
--- next lesson ----
--------------------

12 create folder "contracts/imports"
```
(*) put content of file from:
 https://github.com/ton-blockchain/ton/blob/master/crypto/smartcont/stdlib.fc
```

13 edit file "main.fc"
```
#include "imports/stdlib.fc";

() recv_internal(int my_balance, int msg_value, cell in_msg_full, slice in_msg_body) impure {
    slice cs = in_msg_full.begin_parse();
    int flags = cs~load_uint(4);
    slice sender_address = cs~load_msg_addr();

    set_data(begin_cell().store_slice(sender_address).end_cell());
}

slice get_the_latest_sender() method_id {
    slice ds = get_data().begin_parse();
    return ds~load_msg_addr();
}
```

14 run -> "yarn compile"


--------------------
--- next lesson ----
--------------------

15 add file "scripts/address_convertor.ts"
```
import { Address } from "ton-core";


const address1 = Address.parse('0:a3935861f79daf59a13d6d182e1640210c02f98e3df18fda74b8f5ab141abf18');

// toStrings arguments: urlSafe, bounceable, testOnly
// defaults values: true, true, false

console.log(address1.toString());
console.log(address1.toRawString());
```

16 edit file "package.json"
```
  ...
  ...
  ...
  "scripts": {
    "compile": "ts-node ./scripts/compile.ts",
    "convert": "ts-node ./scripts/address_convertor.ts"
}
```

17 run -> "yarn convert"


--------------------
--- next lesson ----
--------------------

18 run -> "yarn add @ton-community/sandbox jest ts-jest @types/jest ton --dev"

19 create file "jest.config.js"
```
module.exports = {
    preset: "ts-jest",
    testEnviroment: "node",
};
```

19 create folder "tests"
20 create file "tests/main.spec.ts"
```
describe("main.fc contract tests", () => {
    it("our first test", async () => {
        
    });
});
```

21 run -> "yarn jest"

22 edit file "package.js"
```
  ...
  ...
  ...
  "scripts": {
    "compile": "ts-node ./scripts/compile.ts",
    "convert": "ts-node ./scripts/address_convertor.ts",
    "test": "yarn jest"
  }
```

23 run -> "yarn test"


24 create folder "wrappers"
25 create file "wrappers/MainContract.ts"
```
import { Address, Cell, Contract, ContractProvider, SendMode, Sender, beginCell, contractAddress } from "ton-core";
import { receiveMessageOnPort } from "worker_threads";

export class MainContract implements Contract {

    constructor(
        readonly address: Address,
        readonly init?: { code: Cell; data: Cell }
    ) {}

    static createFromConfig(config: any, code: Cell, workchain = 0) {
        const data = beginCell().endCell();
        const init = { code, data };
        const address = contractAddress(workchain, init);

        return new MainContract(address, init);
    }

    async sendInternalMessage(
        provider: ContractProvider,
        sender: Sender,
        value: bigint
    ) {
        await provider.internal(sender, {
            value,
            sendMode: SendMode.PAY_GAS_SEPARATELY,
            body: beginCell().endCell(),
        });
    }

    async getData(provider: ContractProvider) {
        const { stack } = await provider.get("get_the_latest_sender", []);
        return {
            recent_sender: stack.readAddress(),
        };
    }

}
```


46 edit file "tests/main.spec.ts"
```
import { Cell, Address, toNano } from "ton-core";
import { hex } from "../build/main.compiled.json";
import { Blockchain } from "@ton-community/sandbox";
import { MainContract } from "../wrappers/MainContract";

describe("main.fc contract tests", () => {
    it("our first test", async () => {
        const blockchain = await Blockchain.create();

        const codeCell = Cell.fromBoc(Buffer.from(hex, "hex"))[0];  
        
        const myContract = blockchain.openContract(
            await MainContract.createFromConfig({}, codeCell)
        );

        const senderWallet = await blockchain.treasury("sender");

        myContract.sendInternalMessage(senderWallet.getSender(), toNano("0.05"));
    });
});
```

47 run -> "yarn add @ton-community/test-utils -D"

48 edit file "tests/main.spec.ts"
```
import { Cell, Address, toNano } from "ton-core";
import { hex } from "../build/main.compiled.json";
import { Blockchain } from "@ton-community/sandbox";
import { MainContract } from "../wrappers/MainContract";
import "@ton-community/test-utils";

describe("main.fc contract tests", () => {
    it("our first test", async () => {
        const blockchain = await Blockchain.create();

        const codeCell = Cell.fromBoc(Buffer.from(hex, "hex"))[0];  
        
        const myContract = blockchain.openContract(
            await MainContract.createFromConfig({}, codeCell)
        );

        const senderWallet = await blockchain.treasury("sender");

        const sendMessageResult = await myContract.sendInternalMessage(senderWallet.getSender(), toNano("0.05"));
   
        expect(sendMessageResult.transactions).toHaveTransaction({
            from: senderWallet.address,
            to: myContract.address,
            success: true,
        });

        const data = await myContract.getData();

        expect(data.recent_sender.toString()).toBe(senderWallet.address.toString());
   
    });
});
```

49 run -> "yarn add @ton/sandbox jest ts-jest @types/jest @ton/ton --dev"
50 (*) was some mistake in name (*) rename file "jest.config.js" to "jest.config.js"

50 edit file "tests/main.spec.ts" (update imports)
```
import { Cell, toNano } from "@ton/core";
import { hex } from "../build/main.compiled.json";
import { Blockchain } from "@ton/sandbox";
import { MainContract } from "../wrappers/MainContract";
import "@ton/test-utils";

describe("main.fc contract tests", () => {
    it("our first test", async () => {
        const blockchain = await Blockchain.create();

        const codeCell = Cell.fromBoc(Buffer.from(hex, "hex"))[0];  
        
        const myContract = blockchain.openContract(
            await MainContract.createFromConfig({}, codeCell)
        );

        const senderWallet = await blockchain.treasury("sender");

        const sendMessageResult = await myContract.sendInternalMessage(
            senderWallet.getSender(), 
            toNano("0.05"));
   
        expect(sendMessageResult.transactions).toHaveTransaction({
            from: senderWallet.address,
            to: myContract.address,
            success: true,
        });

        const data = await myContract.getData();

        expect(data.recent_sender.toString()).toBe(senderWallet.address.toString());
   
    });
});
```

51 edit file "wrappers/MainContract.ts" (update imports)
```
import { Address, Cell, Contract, ContractProvider, SendMode, Sender, beginCell, contractAddress } from "@ton/core";

export class MainContract implements Contract {

    constructor(
        readonly address: Address,
        readonly init?: { code: Cell; data: Cell }
    ) {}

    static createFromConfig(config: any, code: Cell, workchain = 0) {
        const data = beginCell().endCell();
        const init = { code, data };
        const address = contractAddress(workchain, init);

        return new MainContract(address, init);
    }

    async sendInternalMessage(
        provider: ContractProvider,
        sender: Sender,
        value: bigint
    ) {
        await provider.internal(sender, {
            value,
            sendMode: SendMode.PAY_GAS_SEPARATELY,
            body: beginCell().endCell(),
        });
    }

    async getData(provider: ContractProvider) {
        const { stack } = await provider.get("get_the_latest_sender", []);
        return {
            recent_sender: stack.readAddress(),
        };
    }

}
```

52 run -> "yarn test"

53 add file ".gitignore" from "https://github.com/github/gitignore/blob/main/Node.gitignore"

--------------------
--- next lesson ----
--------------------

54 create file "scripts/deploy.ts"
```
import { Cell, StateInit, beginCell, contractAddress, storeStateInit } from "ton-core";
import { hex } from "../build/main.compiled.json"

async function deployScript() {
    const codeCell = Cell.fromBoc(Buffer.from(hex, "hex"))[0];
    const dataCell = new Cell();

    const stateInit: StateInit = {
        code: codeCell,
        data: dataCell
    };

    const stateInitBuilder = beginCell();
    storeStateInit(stateInit)(stateInitBuilder);
    const stateInitCell = stateInitBuilder.endCell();
    
    const address = contractAddress(0, {
        code: codeCell,
        data: dataCell,
    });
}

deployScript()
```

54 run -> "yarn add qs @types/qs --dev"





