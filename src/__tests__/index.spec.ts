import * as index from '../index'

index.setCoreCreationFlags(index.CoreCreationFlags.DisableAutoLoading)

test('Should have PyScript available', () => {
    expect(index.PyScript).toBeTruthy()
})

test('Should have core available and initialised', () => {
    expect(index.core).toBeTruthy()
})
