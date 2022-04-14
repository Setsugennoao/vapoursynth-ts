export type InferValue<Prop extends PropertyKey, Desc> = Desc extends { get(): any; value: any }
    ? never
    : Desc extends { value: infer T }
    ? Record<Prop, T>
    : Desc extends { get(): infer T }
    ? Record<Prop, T>
    : never

export type DefineProperty<Prop extends PropertyKey, Desc extends PropertyDescriptor> = Desc extends {
    writable: any
    set(val: any): any
}
    ? never
    : Desc extends { writable: any; get(): any }
    ? never
    : Desc extends { writable: false }
    ? Readonly<InferValue<Prop, Desc>>
    : Desc extends { writable: true }
    ? InferValue<Prop, Desc>
    : Readonly<InferValue<Prop, Desc>>
