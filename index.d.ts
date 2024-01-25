export declare class OTManager {
  constructor(isServer: boolean, isQuiet: boolean);

  isServer(): boolean;
  isConnected(): boolean;

  connect(address: string): void;
  disconnect(): void;

  requestData(choiceBits: Uint8Array, numbBlocks: number): Uint8Array;
  respondWithData(data: Uint8Array): void;
}

export function arrayToChoiceBits(arr: ArrayLike<boolean>|ArrayLike<number>): [Uint8Array, number];
