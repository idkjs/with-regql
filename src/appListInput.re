[%bs.raw {|require('./app.css')|}];

[@bs.module] external logo : string = "./logo.svg";


/* aren't strings, nor references, nor some special data type. 
They're called "constructors" (or "tag"). The | bar 
separates each constructor. */
type state = {
  filter:string
};

type action =
  | InputChange(string)
  | EnterPressed;

let query = {|
  query allCompanies($filter: String) {
    companies: allCompanies(filter: 
    { name_contains: $filter
    }) {
      id
      name
    }
  }
|};


type company = {
  id: string,
  name: string
};

type companies = {
  companies:array(company)
};

/* type data = {company:company}; */

let company = (json) =>
  Json.Decode.{
    id: json |> field("id", string),
    name: json |> field("name", string)
  };

let companies = (json) =>
  Json.Decode.{
    companies: json |> field("companies", array(company))
  };
/* let data = (json) => Json.Decode.field("allCompanies", Json.Decode.array(company), json); */

module Container = {
  type shape = companies;
  type variables = {filter:string};
  /* type variables; */
  let decoder = companies;
};

module FetchCompanies = Gql.Client(Container);

/* function to get text value from our input box */
let valueFromEvent = (evt) : string => (
  evt
  |> ReactEventRe.Form.target
  |> ReactDOMRe.domElementToObj
)##value;

/* standalone module for our input element */
module Input = {
  /* For this component, our state type is just string, 
  because that's all we need to keep track of. onChange prop is called with a Form event
  from which we will get the text value and use it as the new state. We have to define valueFromEvent 
  somewhere */
  type state = string;
  let component = ReasonReact.reducerComponent("Input");
  let make = (~onSubmit, _) => {
    ...component,
    initialState: () => "",
    /* We only have one kind of action here, so we don't need to declare a separate action 
    type -- we just use a string, 'newText' */
    reducer: (newText, _text) => ReasonReact.Update(newText),
    render: ({state: text, reduce}) =>
      <input
        value=text
        _type="text"
        placeholder="Search for Company Names"
        onChange=(reduce((evt) => valueFromEvent(evt)))
        onKeyDown=((evt) => 
          if (ReactEventRe.Keyboard.key(evt) == "Enter") {
            onSubmit(text);
            (reduce(() => ""))()
          }
        )
      />
  };
};

let component = ReasonReact.reducerComponent("AppListInput");
let make = (~message, _children) => {
  ...component,
  initialState: () => {filter:""},
  reducer: (action, state) => 
    switch action {
    | InputChange(text) => ReasonReact.Update({...state, filter:text})
    },
  render: ({state, reduce}) => {
    let variables = Some({"filter" : state.filter});
    Js.log(variables);
    Js.log(query);
    /* let variables = {"filter":state}; */
    <div className="App">
      <div className="App-header">
        <img src=logo className="App-logo" alt="logo" />
        <h2> (ReasonReact.stringToElement(message)) </h2>
      </div>
      <p className="App-intro">
        (ReasonReact.stringToElement("To get started, edit"))
        <code> (ReasonReact.stringToElement(" src/app.re ")) </code>
        (ReasonReact.stringToElement("and save to reload."))
      </p>
      <div>
        <Input onSubmit=(reduce((text) => InputChange(text))) />
        <FetchCompanies query variables>
        
          (
            (response) =>
              switch response {
              | Loading => <div> (ReasonReact.stringToElement("Loading")) </div>
              | Failed(error) => <div> (ReasonReact.stringToElement(error)) </div>
              | Loaded(result) => 
                <div> 
                (
                  ReasonReact.arrayToElement(
                    Array.map((company) => <div>(ReasonReact.stringToElement(company.name)) </div>,
                    result.companies)
                  )
                )
                </div> 
              }
          )
        </FetchCompanies>
      </div>
    </div>
  }
};