{
    // Solve the rothalpy equation

    fvScalarMatrix iEqn
    (
        fvm::div(phi, i)
      - fvm::laplacian(turbulence->alphaEff(), i)
     ==
        // Viscous heating: note sign (devRhoReff has a minus in it)
      - (turbulence->devRhoReff() && fvc::grad(Urel))
    );

    iEqn.relax();
    
    fvOptions.constrain(iEqn);

    iEqn.solve();
    
    fvOptions.correct(i);

    // Calculate enthalpy out of rothalpy
    h = i + 0.5*(magSqr(SRF->U()) - magSqr(Urel));
    h.correctBoundaryConditions();

    thermo.correct();
}
