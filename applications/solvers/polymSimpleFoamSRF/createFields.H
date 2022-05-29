Info<< "Reading thermophysical properties\n" << endl;

autoPtr<fluidThermo> pThermo //create the object "thermo"
(
    fluidThermo::New(mesh)
);
fluidThermo& thermo = pThermo();

volScalarField& h = thermo.he();

volScalarField& p = thermo.p();

volScalarField rho //create scalar field rho
(
    IOobject
    (
        "rho",
        runTime.timeName(),
        mesh,
        IOobject::READ_IF_PRESENT,
        IOobject::AUTO_WRITE
    ),
    thermo.rho()
);

Info<< "Reading field Urel\n" << endl; //create vector field U
volVectorField Urel
(
    IOobject
    (
        "Urel",
        runTime.timeName(),
        mesh,
        IOobject::MUST_READ,
        IOobject::AUTO_WRITE
    ),
    mesh
);

//creates and initializes the relative face-flux field phi
Info<< "Reading/calculating face flux field phi\n" << endl;

surfaceScalarField phi
(
    IOobject
    (
        "phi",
        runTime.timeName(),
        mesh,
        IOobject::READ_IF_PRESENT,
        IOobject::AUTO_WRITE
    ),
    linearInterpolate(rho*Urel) & mesh.Sf()
);

pressureControl pressureControl(p, rho, simple.dict());

mesh.setFluxRequired(p.name());

Info<< "Creating turbulence model\n" << endl; 
autoPtr<compressible::turbulenceModel> turbulence //turbulence model is created  --> Foam::compressible::turbulenceModel  steckt in turbulentFluidThermoModel.H
(
    compressible::turbulenceModel::New
    (
        rho,
        Urel,
        phi,
        thermo
    )
);

Info<< "Creating SRF model\n" << endl;
autoPtr<SRF::SRFModel> SRF(SRF::SRFModel::New(Urel));

// Construct the absolute velocity
volVectorField U
(
    IOobject
    (
        "U",
        runTime.timeName(),
        mesh,
        IOobject::NO_READ,
        IOobject::AUTO_WRITE
    ),
    Urel + SRF->U()
);

// Create rothalpy, in two steps to preserve boundary conditions
    volScalarField i
    (
        IOobject
        (
            "i",
            runTime.timeName(),
            mesh,
            IOobject::NO_READ,
            IOobject::AUTO_WRITE
        ),
        h
    );
    i == h - 0.5*(magSqr(SRF->U()) - magSqr(Urel));

dimensionedScalar initialMass = fvc::domainIntegrate(rho);

#include "createFvOptions.H"
